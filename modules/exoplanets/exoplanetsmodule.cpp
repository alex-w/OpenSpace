/*****************************************************************************************
 *                                                                                       *
 * OpenSpace                                                                             *
 *                                                                                       *
 * Copyright (c) 2014-2025                                                               *
 *                                                                                       *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this  *
 * software and associated documentation files (the "Software"), to deal in the Software *
 * without restriction, including without limitation the rights to use, copy, modify,    *
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to    *
 * permit persons to whom the Software is furnished to do so, subject to the following   *
 * conditions:                                                                           *
 *                                                                                       *
 * The above copyright notice and this permission notice shall be included in all copies *
 * or substantial portions of the Software.                                              *
 *                                                                                       *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,   *
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A         *
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT    *
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF  *
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE  *
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                         *
 ****************************************************************************************/

#include <modules/exoplanets/exoplanetsmodule.h>

#include <modules/exoplanets/datastructure.h>
#include <modules/exoplanets/exoplanetshelper.h>
#include <modules/exoplanets/rendering/renderableorbitdisc.h>
#include <modules/exoplanets/tasks/exoplanetsdatapreparationtask.h>
#include <openspace/engine/globals.h>
#include <openspace/engine/globalscallbacks.h>
#include <openspace/engine/moduleengine.h>
#include <openspace/query/query.h>
#include <openspace/rendering/renderengine.h>
#include <openspace/scene/scenegraphnode.h>
#include <openspace/scripting/scriptengine.h>
#include <openspace/util/distanceconstants.h>
#include <openspace/util/factorymanager.h>
#include <openspace/util/timeconversion.h>
#include <openspace/util/timemanager.h>
#include <ghoul/filesystem/filesystem.h>
#include <ghoul/format.h>
#include <ghoul/glm.h>
#include <ghoul/logging/logmanager.h>
#include <ghoul/misc/assert.h>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "exoplanetsmodule_lua.inl"

namespace {
    constexpr openspace::properties::Property::PropertyInfo DataFolderInfo = {
        "DataFolder",
        "Data Folder",
        "The path to the folder containing the exoplanets data and lookup table.",
        openspace::properties::Property::Visibility::AdvancedUser
    };

    constexpr openspace::properties::Property::PropertyInfo BvColorMapInfo = {
        "BvColormap",
        "B-V Colormap",
        "The path to a cmap file that maps a B-V color index to an RGB color.",
        openspace::properties::Property::Visibility::AdvancedUser
    };

    constexpr openspace::properties::Property::PropertyInfo StarTextureInfo = {
        "StarTexture",
        "Star Texture",
        "The path to a grayscale image that is used for the host star surfaces.",
        openspace::properties::Property::Visibility::AdvancedUser
    };

    constexpr openspace::properties::Property::PropertyInfo StarGlareTextureInfo = {
        "StarGlareTexture",
        "Star Glare Texture",
        "The path to a grayscale image that is used for the glare effect of the "
        "host stars.",
        openspace::properties::Property::Visibility::AdvancedUser
    };

    constexpr openspace::properties::Property::PropertyInfo NoDataTextureInfo = {
        "NoDataTexture",
        "No Data Star Texture",
        "A path to a texture that is used to represent that there is missing data about "
        "the star. For example no color information.",
        openspace::properties::Property::Visibility::AdvancedUser
    };

    constexpr openspace::properties::Property::PropertyInfo PlanetDefaultTextureInfo = {
        "PlanetDefaultTexture",
        "Planet Default Texture",
        "The path to an image that should be used by default for the planets in all "
        "added exoplanet systems. If not specified, the planets are rendered without a "
        "texture when added.",
        openspace::properties::Property::Visibility::AdvancedUser
    };

    constexpr openspace::properties::Property::PropertyInfo OrbitDiscTextureInfo = {
        "OrbitDiscTexture",
        "Orbit Disc Texture",
        "A path to a 1-dimensional image used as a transfer function for the "
        "exoplanets' orbit uncertainty disc.",
        openspace::properties::Property::Visibility::AdvancedUser
    };

    constexpr openspace::properties::Property::PropertyInfo HabitableZoneTextureInfo = {
        "HabitableZoneTexture",
        "Habitable Zone Texture",
        "A path to a 1-dimensional image used as a transfer function for the "
        "habitable zone disc.",
        openspace::properties::Property::Visibility::AdvancedUser
    };

    constexpr openspace::properties::Property::PropertyInfo ComparisonCircleColorInfo = {
        "ComparisonCircleColor",
        "Comparison Circle Color",
        "Decides the color of the 1 AU size comparison circles that are generated as "
        "part of an exoplanet system. Changing the color will not modify already "
        "existing circles.",
        openspace::properties::Property::Visibility::NoviceUser
    };

    constexpr openspace::properties::Property::PropertyInfo ShowComparisonCircleInfo = {
        "ShowComparisonCircle",
        "Show Comparison Circle",
        "If true, the 1 AU size comparison circle is enabled per default when an "
        "exoplanet system is created.",
        openspace::properties::Property::Visibility::NoviceUser
    };

    constexpr openspace::properties::Property::PropertyInfo ShowOrbitUncertaintyInfo = {
        "ShowOrbitUncertainty",
        "Show Orbit Uncertainty",
        "If true, a disc showing the uncertainty for each planetary orbit is enabled per "
        "default when an exoplanet system is created.",
        openspace::properties::Property::Visibility::User
    };

    constexpr openspace::properties::Property::PropertyInfo ShowHabitableZoneInfo = {
        "ShowHabitableZone",
        "Show Habitable Zone",
        "If true, the habitable zone disc is enabled per default when an exoplanet "
        "system is created.",
        openspace::properties::Property::Visibility::User
    };

    constexpr openspace::properties::Property::PropertyInfo UseOptimisticZoneInfo = {
        "UseOptimisticZone",
        "Use Optimistic Zone Boundaries",
        "If true, the habitable zone is computed with optimistic boundaries per default "
        "when an exoplanet system is created.",
        openspace::properties::Property::Visibility::User
    };

    constexpr openspace::properties::Property::PropertyInfo HabitableZoneOpacityInfo = {
        "HabitableZoneOpacity",
        "Habitable Zone Opacity",
        "The opacity value used for the habitable zone renderable for a created "
        "exoplanet system.",
        openspace::properties::Property::Visibility::NoviceUser
    };

    constexpr std::string_view ExoplanetsDataFileName = "exoplanets_data.bin";
    constexpr std::string_view LookupTableFileName = "lookup.txt";
    constexpr std::string_view TeffToBvConversionFileName = "teff_bv.txt";

    struct [[codegen::Dictionary(ExoplanetsModule)]] Parameters {
        // [[codegen::verbatim(DataFolderInfo.description)]]
        std::optional<std::filesystem::path> dataFolder [[codegen::directory()]];

        // [[codegen::verbatim(BvColorMapInfo.description)]]
        std::optional<std::filesystem::path> bvColormap;

       // [[codegen::verbatim(StarTextureInfo.description)]]
       std::optional<std::filesystem::path> starTexture;

       // [[codegen::verbatim(StarGlareTextureInfo.description)]]
       std::optional<std::filesystem::path> starGlareTexture;

       // [[codegen::verbatim(NoDataTextureInfo.description)]]
       std::optional<std::filesystem::path> noDataTexture;

       // [[codegen::verbatim(PlanetDefaultTextureInfo.description)]]
       std::optional<std::filesystem::path> planetDefaultTexture;

       // [[codegen::verbatim(OrbitDiscTextureInfo.description)]]
       std::optional<std::filesystem::path> orbitDiscTexture;

       // [[codegen::verbatim(HabitableZoneTextureInfo.description)]]
       std::optional<std::filesystem::path> habitableZoneTexture;

       // [[codegen::verbatim(ComparisonCircleColorInfo.description)]]
       std::optional<glm::vec3> comparisonCircleColor [[codegen::color()]];

       // [[codegen::verbatim(ShowComparisonCircleInfo.description)]]
       std::optional<bool> showComparisonCircle;

       // [[codegen::verbatim(ShowOrbitUncertaintyInfo.description)]]
       std::optional<bool> showOrbitUncertainty;

       // [[codegen::verbatim(ShowHabitableZoneInfo.description)]]
       std::optional<bool> showHabitableZone;

       // [[codegen::verbatim(UseOptimisticZoneInfo.description)]]
       std::optional<bool> useOptimisticZone;

       // [[codegen::verbatim(HabitableZoneOpacityInfo.description)]]
       std::optional<float> habitableZoneOpacity [[codegen::inrange(0, 1)]];
    };
#include "exoplanetsmodule_codegen.cpp"
} // namespace

namespace openspace {

using namespace exoplanets;

documentation::Documentation ExoplanetsModule::Documentation() {
    return codegen::doc<Parameters>("module_exoplanets");
}

ExoplanetsModule::ExoplanetsModule()
    : OpenSpaceModule(Name)
    , _exoplanetsDataFolder(DataFolderInfo)
    , _bvColorMapPath(BvColorMapInfo)
    , _starTexturePath(StarTextureInfo)
    , _starGlareTexturePath(StarGlareTextureInfo)
    , _noDataTexturePath(NoDataTextureInfo)
    , _planetDefaultTexturePath(PlanetDefaultTextureInfo)
    , _orbitDiscTexturePath(OrbitDiscTextureInfo)
    , _habitableZoneTexturePath(HabitableZoneTextureInfo)
    , _comparisonCircleColor(
        ComparisonCircleColorInfo,
        glm::vec3(0.f, 0.8f, 0.8f),
        glm::vec3(0.f),
        glm::vec3(1.f)
    )
    , _showComparisonCircle(ShowComparisonCircleInfo, false)
    , _showOrbitUncertainty(ShowOrbitUncertaintyInfo, true)
    , _showHabitableZone(ShowHabitableZoneInfo, true)
    , _useOptimisticZone(UseOptimisticZoneInfo, true)
    , _habitableZoneOpacity(HabitableZoneOpacityInfo, 0.1f, 0.f, 1.f)
{
    _exoplanetsDataFolder.setReadOnly(true);

    _exoplanetsDataFolder.onChange([this]() {
        std::filesystem::path f = _exoplanetsDataFolder.value();
       if (!std::filesystem::is_directory(f)) {
            LERROR(std::format(
                "Could not find directory: '{}' for module setting '{}'",
                f, _exoplanetsDataFolder.identifier()
            ));
        }
    });
    addProperty(_exoplanetsDataFolder);

    auto createPathOnChange = [](const properties::StringProperty& p) {
        return [&p]() {
            std::filesystem::path f = p.value();
            if (!std::filesystem::is_regular_file(f)) {
                LERROR(std::format(
                    "Could not find file: '{}' for module setting '{}'",
                    f, p.identifier()
                ));
            }
        };
    };
    _bvColorMapPath.onChange(createPathOnChange(_bvColorMapPath));
    addProperty(_bvColorMapPath);

    _starTexturePath.onChange(createPathOnChange(_starTexturePath));
    addProperty(_starTexturePath);

    _starGlareTexturePath.onChange(createPathOnChange(_starGlareTexturePath));
    addProperty(_starGlareTexturePath);

    _noDataTexturePath.onChange(createPathOnChange(_noDataTexturePath));
    addProperty(_noDataTexturePath);

    _planetDefaultTexturePath.onChange(createPathOnChange(_planetDefaultTexturePath));
    addProperty(_planetDefaultTexturePath);

    _orbitDiscTexturePath.onChange(createPathOnChange(_orbitDiscTexturePath));
    addProperty(_orbitDiscTexturePath);

    _habitableZoneTexturePath.onChange(createPathOnChange(_habitableZoneTexturePath));
    addProperty(_habitableZoneTexturePath);

    _comparisonCircleColor.setViewOption(properties::Property::ViewOptions::Color);
    addProperty(_comparisonCircleColor);
    addProperty(_showComparisonCircle);
    addProperty(_showOrbitUncertainty);
    addProperty(_showHabitableZone);
    addProperty(_useOptimisticZone);

    addProperty(_habitableZoneOpacity);
}

bool ExoplanetsModule::hasDataFiles() const {
    return !_exoplanetsDataFolder.value().empty();
}

std::filesystem::path ExoplanetsModule::exoplanetsDataPath() const {
    ghoul_assert(hasDataFiles(), "Data files not loaded");

    return absPath(
        std::format("{}/{}", _exoplanetsDataFolder.value(), ExoplanetsDataFileName)
    );
}

std::filesystem::path ExoplanetsModule::lookUpTablePath() const {
    ghoul_assert(hasDataFiles(), "Data files not loaded");

    return absPath(
        std::format("{}/{}", _exoplanetsDataFolder.value(), LookupTableFileName)
    );
}

std::filesystem::path ExoplanetsModule::teffToBvConversionFilePath() const {
    ghoul_assert(hasDataFiles(), "Data files not loaded");

    return absPath(std::format(
        "{}/{}", _exoplanetsDataFolder.value(), TeffToBvConversionFileName
    ));
}

std::filesystem::path ExoplanetsModule::bvColormapPath() const {
    return _bvColorMapPath.value();
}

std::filesystem::path ExoplanetsModule::starTexturePath() const {
    return _starTexturePath.value();
}

std::filesystem::path ExoplanetsModule::starGlareTexturePath() const {
    return _starGlareTexturePath.value();
}

std::filesystem::path ExoplanetsModule::noDataTexturePath() const {
    return _noDataTexturePath.value();
}

std::filesystem::path ExoplanetsModule::planetDefaultTexturePath() const {
    return _planetDefaultTexturePath.value();
}

std::filesystem::path ExoplanetsModule::orbitDiscTexturePath() const {
    return _orbitDiscTexturePath.value();
}

std::filesystem::path ExoplanetsModule::habitableZoneTexturePath() const {
    return _habitableZoneTexturePath.value();
}

glm::vec3 ExoplanetsModule::comparisonCircleColor() const {
    return _comparisonCircleColor;
}

bool ExoplanetsModule::showComparisonCircle() const {
    return _showComparisonCircle;
}

bool ExoplanetsModule::showOrbitUncertainty() const {
    return _showOrbitUncertainty;
}

bool ExoplanetsModule::showHabitableZone() const {
    return _showHabitableZone;
}

bool ExoplanetsModule::useOptimisticZone() const {
    return _useOptimisticZone;
}

float ExoplanetsModule::habitableZoneOpacity() const {
    return _habitableZoneOpacity;
}

void ExoplanetsModule::internalInitialize(const ghoul::Dictionary& dict) {
    const Parameters p = codegen::bake<Parameters>(dict);

    if (p.dataFolder.has_value()) {
        _exoplanetsDataFolder = p.dataFolder->string();
    }

    if (p.bvColormap.has_value()) {
        _bvColorMapPath = p.bvColormap->string();
    }

    if (p.starTexture.has_value()) {
        _starTexturePath = p.starTexture->string();
    }

    if (p.starGlareTexture.has_value()) {
        _starGlareTexturePath = p.starGlareTexture->string();
    }

    if (p.planetDefaultTexture.has_value()) {
        _planetDefaultTexturePath = p.planetDefaultTexture->string();
    }

    if (p.noDataTexture.has_value()) {
        _noDataTexturePath = p.noDataTexture->string();
    }

    if (p.orbitDiscTexture.has_value()) {
        _orbitDiscTexturePath = p.orbitDiscTexture->string();
    }

    if (p.habitableZoneTexture.has_value()) {
        _habitableZoneTexturePath = p.habitableZoneTexture->string();
    }

    _comparisonCircleColor = p.comparisonCircleColor.value_or(_comparisonCircleColor);
    _showComparisonCircle = p.showComparisonCircle.value_or(_showComparisonCircle);
    _showOrbitUncertainty = p.showOrbitUncertainty.value_or(_showOrbitUncertainty);
    _showHabitableZone = p.showHabitableZone.value_or(_showHabitableZone);
    _useOptimisticZone = p.useOptimisticZone.value_or(_useOptimisticZone);

    _habitableZoneOpacity = p.habitableZoneOpacity.value_or(_habitableZoneOpacity);

    ghoul::TemplateFactory<Task>* fTask = FactoryManager::ref().factory<Task>();
    ghoul::TemplateFactory<Renderable>* fRenderable =
        FactoryManager::ref().factory<Renderable>();
    ghoul_assert(fTask, "No task factory existed");
    fTask->registerClass<ExoplanetsDataPreparationTask>("ExoplanetsDataPreparationTask");
    fRenderable->registerClass<RenderableOrbitDisc>("RenderableOrbitDisc");
}

std::vector<documentation::Documentation> ExoplanetsModule::documentations() const {
    return {
        ExoplanetsDataPreparationTask::documentation(),
        RenderableOrbitDisc::Documentation(),
        ExoplanetSystem::Documentation()
    };
}

scripting::LuaLibrary ExoplanetsModule::luaLibrary() const {
    return {
        .name = "exoplanets",
        .functions = {
            codegen::lua::RemoveExoplanetSystem,
            codegen::lua::SystemData,
            codegen::lua::ListOfExoplanets,
            codegen::lua::ListAvailableExoplanetSystems,
            codegen::lua::LoadSystemDataFromCsv
        },
        .scripts = {
            absPath("${MODULE_EXOPLANETS}/scripts/systemcreation.lua")
        }
    };
}

} // namespace openspace
