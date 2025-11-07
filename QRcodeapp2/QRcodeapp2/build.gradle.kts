// 透過 toml 檔案宣告外掛程式，這樣可以讓版本集中管理。
// 'apply false' 表示不在根專案中實際應用，而是在需要它的子模組（如 :app）中應用。
plugins {
    alias(libs.plugins.android.application) apply false
    alias(libs.plugins.kotlin.android) apply false
    alias(libs.plugins.kotlin.compose) apply false
}
