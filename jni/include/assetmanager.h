/*
   AssetManager for Android NDK
   Copyright (c) 2006-2013 SIProp Project http://www.siprop.org/

   This software is provided 'as-is', without any express or implied warranty.
   In no event will the authors be held liable for any damages arising from the use of this software.
   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it freely,
   subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
   2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
   3. This notice may not be removed or altered from any source distribution.
*/

#ifndef ASSETMANAGER_APK_H_
#define ASSETMANAGER_APK_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Copy "assets" directory from APK file to under "/data/data/[Package Name]" directory.
 *
 * @param package_name Package Name
 * @param apk_name APK File Name without ".apk"
 */
int setupAsset_setAPKname(const char *package_name, const char *apk_name);

/**
 * Copy "assets" directory from APK file to under "/data/data/[Package Name]" directory.
 *
 * @param package_name Package Name
 */
int setupAsset(const char *package_name);

/**
 * Copy File of "load_file_name" from APK file to under "/data/data/[Package Name]/assets" directory.
 *
 * @param package_name Package Name
 * @param apk_name APK File Name without ".apk"
 * @param load_file_name from APK File to under "/data/data/[Package Name]" dir.@Ex: File Path in Eclipse as "assets/image/test.jpg" -> "image/test.jpg"
 */
int loadAseetFile_setAPKname(const char *package_name, const char *apk_name, const char *load_file_name);

/**
 * Copy File of "load_file_name" from APK file to under "/data/data/[Package Name]/assets" directory.
 *
 * @param package_name Package Name
 * @param load_file_name from APK File to under "/data/data/[Package Name]" dir.@Ex: File Path in Eclipse as "assets/image/test.jpg" -> "image/test.jpg"
 */
int loadAseetFile(const char *package_name, const char *load_file_name);

#ifdef __cplusplus
}
#endif

#endif /* ASSETMANAGER_APK_H_ */
