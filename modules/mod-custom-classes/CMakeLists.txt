#
# This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
# more details.
#
# You should have received a copy of the GNU General Public License along
# with this program. If not, see <http://www.gnu.org/licenses/>.
#

set(MODULE_NAME mod-custom-classes)
MESSAGE(STATUS "Configuring module: ${MODULE_NAME}")

# Collect all source files from src/
CollectSourceFiles(${CMAKE_CURRENT_SOURCE_DIR}/src PRIVATE_SOURCES_MODULES)
CollectIncludeDirectories(${CMAKE_CURRENT_SOURCE_DIR} PUBLIC_INCLUDES)

# Register this module as a static module
list(APPEND STATIC_SCRIPT_MODULES ${MODULE_NAME})
