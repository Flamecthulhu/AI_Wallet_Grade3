plugins {
    // 不再使用 alias，直接用 ID
    id("com.android.application")
    id("org.jetbrains.kotlin.android")
    id("org.jetbrains.kotlin.plugin.compose")
}


android {
    namespace = "com.example.qrcodeapp"
    compileSdk = 34// ⬅️ 修改

    defaultConfig {
        applicationId = "com.example.qrcodeapp"
        minSdk = 24
        targetSdk = 34 // ⬅️ 修改
        versionCode = 1
        versionName = "1.0"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"

        ndk {
            abiFilters += listOf("armeabi-v7a", "arm64-v8a", "x86_64")
        }
    }

    packaging {
        jniLibs {
            useLegacyPackaging = true
        }
        // ▼ 解決 ML Kit 可能引起的 packaging 衝突 ▼
        resources {
            excludes += "/META-INF/{AL2.0,LGPL2.1}"
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_1_8 // ⬅️ 建議改為 1.8
        targetCompatibility = JavaVersion.VERSION_1_8 // ⬅️ 建議改為 1.8
    }
    kotlinOptions {
        jvmTarget = "1.8" // ⬅️ 建議改為 1.8
    }
    buildFeatures {
        compose = true
    }
    composeOptions {
        // 這個版本號必須和你的 Kotlin 版本對應
        kotlinCompilerExtensionVersion = libs.versions.kotlinCompilerExtension.get()
    }
}

dependencies {
    implementation(libs.androidx.core.ktx)
    implementation(libs.androidx.lifecycle.runtime.ktx)
    implementation(libs.androidx.activity.compose)
    implementation(platform(libs.androidx.compose.bom))
    implementation(libs.androidx.compose.ui)
    implementation(libs.androidx.compose.ui.graphics)
    implementation(libs.androidx.compose.ui.tooling.preview)
    implementation(libs.androidx.compose.material3)
    testImplementation(libs.junit)
    androidTestImplementation(libs.androidx.junit)
    androidTestImplementation(libs.androidx.espresso.core)
    androidTestImplementation(platform(libs.androidx.compose.bom))
    androidTestImplementation(libs.androidx.compose.ui.test.junit4)
    debugImplementation(libs.androidx.compose.ui.tooling)
    debugImplementation(libs.androidx.compose.ui.test.manifest)

    // ▼ 使用 TOML 中定義的別名 ▼
    // ML Kit 條碼掃描
    implementation(libs.mlkit.barcode.scanning)

    // CameraX
    implementation(libs.androidx.camera.camera2)
    implementation(libs.androidx.camera.lifecycle)
    implementation(libs.androidx.camera.view)

    // 權限處理 (Accompanist 已被棄用，Compose Foundation 內建)
    // 我們將在 MainActivity.kt 中使用新的 import

    // Material Icons (若需要)
    implementation(libs.androidx.compose.material.icons.extended)
    implementation("com.google.accompanist:accompanist-permissions:0.32.0")


    // 假設你在 toml 中也定義了這個
}
