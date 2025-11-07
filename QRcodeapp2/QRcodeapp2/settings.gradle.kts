// 檔案： D:\QRcodeapp2\settings.gradle.kts

// ⬇️ 這是你新加入的區塊
// 它告訴 Gradle 去哪裡尋找「插件」
pluginManagement {
    repositories {
        google()
        mavenCentral()
        gradlePluginPortal() // 預設的插件商店
    }
}

// 這是你原有的區塊
// 它告訴 Gradle 去哪裡尋找「函式庫 (dependencies)」
dependencyResolutionManagement {
    repositoriesMode.set(RepositoriesMode.FAIL_ON_PROJECT_REPOS)
    repositories {
        google()
        mavenCentral()
    }
}

rootProject.name = "QRcodeapp2"
include(":app")