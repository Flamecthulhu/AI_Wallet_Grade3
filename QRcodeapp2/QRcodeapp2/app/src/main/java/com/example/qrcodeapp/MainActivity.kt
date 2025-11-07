package com.example.qrcodeapp
// ... 其他 import ...import androidx.activity.compose.setContent // ⬅️ 加入這個 import
import android.os.Bundle
import androidx.activity.compose.setContent
import androidx.compose.foundation.background
import androidx.compose.foundation.isSystemInDarkTheme // ⬅️ 加入這個 import
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.size
import androidx.compose.material3.darkColorScheme // ⬅️ 加入這行
import androidx.compose.material3.lightColorScheme // ⬅️ 加入這行
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
//import androidx.wear.compose.foundation.weight
import com.google.accompanist.permissions.ExperimentalPermissionsApi
import com.google.accompanist.permissions.PermissionStatus // ⬅️ 加入這個 import
import com.google.accompanist.permissions.rememberPermissionState
import com.google.accompanist.permissions.isGranted // ⬅️ 加入這個 import
import androidx.activity.ComponentActivity
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.NoPhotography
import androidx.compose.foundation.layout.*



// ... CameraPreview 和其他 Composable ...

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            // 建議加上一個主題
            MaterialTheme {
                ScanScreen()
            }
        }
    }
}

// ... 其他 Composable ...

// ==================== 掃描畫面 ====================

@OptIn(ExperimentalPermissionsApi::class)
@Composable
fun ScanScreen() {
    var scannedText by remember { mutableStateOf("") }
    var scanHistory by remember { mutableStateOf(listOf<String>()) }
    var isConnected by remember { mutableStateOf(false) }

    val cameraPermissionState = rememberPermissionState(android.Manifest.permission.CAMERA)

    LaunchedEffect(key1 = true) {
        // ▼ 修正 API 用法 ▼
        if (!cameraPermissionState.status.isGranted) {
            cameraPermissionState.launchPermissionRequest()
        }
    }
    Column(
        modifier = Modifier
            .fillMaxSize()
            .background(Color(0xFF1E1E1E))
    ) {
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .weight(0.5f)
                .background(Color(0xFF2C2C2C)),
            contentAlignment = Alignment.Center
        ) {
            // ▼ 修正 API 用法 ▼
            if (cameraPermissionState.status.isGranted) {
                CameraPreview(
                    modifier = Modifier.fillMaxSize(),
                    onBarcodeScanned = { barcode ->
                        if (scannedText != barcode) {
                            scannedText = barcode
                            scanHistory = listOf(barcode) + scanHistory.take(9)
                        }
                    }
                )
            }
            // ... 其餘程式碼不變 ...
            else {
                Column(
                    horizontalAlignment = Alignment.CenterHorizontally
                ) {
                    Icon(
                        imageVector = Icons.Default.NoPhotography,
                        contentDescription = "No Camera",
                        modifier = Modifier.size(100.dp),
                        tint = Color.Gray
                    )
                    Spacer(modifier = Modifier.height(16.dp))
                    Text(
                        text = "請授予相機權限",
                        color = Color.Gray,
                        fontSize = 16.sp
                    )
                }
            }
        }
        // ... 其餘程式碼不變 ...
    }
}
// ... 其他 Composable ...
@Composable
fun DrawerContent(
    currentScreen: String,
    onScreenSelected: (String) -> Unit
) {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .background(Color(0xFF1E1E1E))
    ) {
        // 選單頂部 (App 資訊)
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .background(Color(0xFF2196F3))
                .padding(24.dp)
        ) {
            Column {
                Icon(
                    imageVector = Icons.Default.QrCodeScanner,
                    contentDescription = null,
                    modifier = Modifier.size(48.dp),
                    tint = Color.White
                )
                Spacer(modifier = Modifier.height(8.dp))
                Text(
                    text = "QR Scanner",
                    fontSize = 20.sp,
                    fontWeight = FontWeight.Bold,
                    color = Color.White
                )
                Text(
                    text = "版本 1.0.0",
                    fontSize = 14.sp,
                    color = Color.White.copy(alpha = 0.8f)
                )
            }
        }

        Spacer(modifier = Modifier.height(8.dp))

        // 選單項目
        DrawerMenuItem(
            icon = Icons.Default.QrCodeScanner,
            title = "掃描",
            isSelected = currentScreen == "scan",
            onClick = { onScreenSelected("scan") }
        )

        DrawerMenuItem(
            icon = Icons.Default.Settings,
            title = "設定",
            isSelected = currentScreen == "settings",
            onClick = { onScreenSelected("settings") }
        )

        DrawerMenuItem(
            icon = Icons.Default.Info,
            title = "關於",
            isSelected = currentScreen == "about",
            onClick = { onScreenSelected("about") }
        )

        Spacer(modifier = Modifier.weight(1f))

        // 底部資訊
        Divider(color = Color.Gray.copy(alpha = 0.3f))

        Text(
            text = "© 2024 QR Scanner",
            fontSize = 12.sp,
            color = Color.Gray,
            modifier = Modifier.padding(16.dp)
        )
    }
}

@Composable
fun DrawerMenuItem(
    icon: ImageVector,
    title: String,
    isSelected: Boolean,
    onClick: () -> Unit
) {
    Box(
        modifier = Modifier
            .fillMaxWidth()
            .clickable(onClick = onClick)
            .background(
                if (isSelected) Color(0xFF2196F3).copy(alpha = 0.2f)
                else Color.Transparent
            )
            .padding(horizontal = 16.dp, vertical = 12.dp)
    ) {
        Row(
            verticalAlignment = Alignment.CenterVertically,
            modifier = Modifier.fillMaxWidth()
        ) {
            Icon(
                imageVector = icon,
                contentDescription = title,
                tint = if (isSelected) Color(0xFF2196F3) else Color.White,
                modifier = Modifier.size(24.dp)
            )
            Spacer(modifier = Modifier.width(16.dp))
            Text(
                text = title,
                fontSize = 16.sp,
                color = if (isSelected) Color(0xFF2196F3) else Color.White,
                fontWeight = if (isSelected) FontWeight.Bold else FontWeight.Normal
            )
        }
    }
}

// ==================== 掃描畫面 ====================


// ==================== 設定畫面 ====================
@Composable
fun SettingsScreen() {
    var autoConnect by remember { mutableStateOf(true) }
    var soundEnabled by remember { mutableStateOf(true) }
    var vibrationEnabled by remember { mutableStateOf(true) }
    var saveHistory by remember { mutableStateOf(true) }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .background(Color(0xFF1E1E1E))
            .padding(16.dp)
    ) {
        Text(
            text = "一般設定",
            fontSize = 20.sp,
            fontWeight = FontWeight.Bold,
            color = Color.White,
            modifier = Modifier.padding(bottom = 16.dp)
        )

        Card(
            modifier = Modifier.fillMaxWidth(),
            colors = CardDefaults.cardColors(
                containerColor = Color(0xFF2C2C2C)
            )
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                SettingItem(
                    title = "自動連線",
                    subtitle = "啟動時自動連線到 STM32",
                    checked = autoConnect,
                    onCheckedChange = { autoConnect = it }
                )

                Divider(
                    color = Color.Gray.copy(alpha = 0.3f),
                    modifier = Modifier.padding(vertical = 12.dp)
                )

                SettingItem(
                    title = "掃描音效",
                    subtitle = "掃描成功時播放提示音",
                    checked = soundEnabled,
                    onCheckedChange = { soundEnabled = it }
                )

                Divider(
                    color = Color.Gray.copy(alpha = 0.3f),
                    modifier = Modifier.padding(vertical = 12.dp)
                )

                SettingItem(
                    title = "震動回饋",
                    subtitle = "掃描成功時震動提示",
                    checked = vibrationEnabled,
                    onCheckedChange = { vibrationEnabled = it }
                )

                Divider(
                    color = Color.Gray.copy(alpha = 0.3f),
                    modifier = Modifier.padding(vertical = 12.dp)
                )

                SettingItem(
                    title = "儲存歷史記錄",
                    subtitle = "保留最近 100 筆掃描記錄",
                    checked = saveHistory,
                    onCheckedChange = { saveHistory = it }
                )
            }
        }

        Spacer(modifier = Modifier.height(24.dp))

        Text(
            text = "藍牙設定",
            fontSize = 20.sp,
            fontWeight = FontWeight.Bold,
            color = Color.White,
            modifier = Modifier.padding(bottom = 16.dp)
        )

        Card(
            modifier = Modifier.fillMaxWidth(),
            colors = CardDefaults.cardColors(
                containerColor = Color(0xFF2C2C2C)
            )
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Text(
                    text = "STM32 裝置位址",
                    color = Color.White,
                    fontSize = 14.sp,
                    fontWeight = FontWeight.Bold
                )
                Spacer(modifier = Modifier.height(8.dp))
                Text(
                    text = "00:11:22:33:44:55",
                    color = Color.Gray,
                    fontSize = 14.sp
                )

                Spacer(modifier = Modifier.height(16.dp))

                Button(
                    onClick = { /* 掃描藍牙裝置 */ },
                    modifier = Modifier.fillMaxWidth(),
                    colors = ButtonDefaults.buttonColors(
                        containerColor = Color(0xFF2196F3)
                    )
                ) {
                    Icon(Icons.Default.Bluetooth, contentDescription = null)
                    Spacer(modifier = Modifier.width(8.dp))
                    Text("搜尋藍牙裝置")
                }
            }
        }

        Spacer(modifier = Modifier.height(24.dp))

        Button(
            onClick = { /* 清除快取 */ },
            modifier = Modifier.fillMaxWidth(),
            colors = ButtonDefaults.buttonColors(
                containerColor = Color(0xFFF44336)
            )
        ) {
            Text("清除所有資料")
        }
    }
}

@Composable
fun SettingItem(
    title: String,
    subtitle: String,
    checked: Boolean,
    onCheckedChange: (Boolean) -> Unit
) {
    Row(
        modifier = Modifier.fillMaxWidth(),
        horizontalArrangement = Arrangement.SpaceBetween,
        verticalAlignment = Alignment.CenterVertically
    ) {
        Column(modifier = Modifier.weight(1f)) {
            Text(
                text = title,
                color = Color.White,
                fontSize = 16.sp,
                fontWeight = FontWeight.Medium
            )
            Spacer(modifier = Modifier.height(4.dp))
            Text(
                text = subtitle,
                color = Color.Gray,
                fontSize = 12.sp
            )
        }
        Switch(
            checked = checked,
            onCheckedChange = onCheckedChange,
            colors = SwitchDefaults.colors(
                checkedThumbColor = Color.White,
                checkedTrackColor = Color(0xFF2196F3),
                uncheckedThumbColor = Color.Gray,
                uncheckedTrackColor = Color(0xFF3C3C3C)
            )
        )
    }
}

// ==================== 關於畫面 ====================
@Composable
fun AboutScreen() {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .background(Color(0xFF1E1E1E))
            .padding(24.dp),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Spacer(modifier = Modifier.height(32.dp))

        // App 圖示
        Icon(
            imageVector = Icons.Default.QrCodeScanner,
            contentDescription = null,
            modifier = Modifier.size(100.dp),
            tint = Color(0xFF2196F3)
        )

        Spacer(modifier = Modifier.height(24.dp))

        Text(
            text = "QR Code 掃描器",
            fontSize = 28.sp,
            fontWeight = FontWeight.Bold,
            color = Color.White
        )

        Spacer(modifier = Modifier.height(8.dp))

        Text(
            text = "版本 1.0.0",
            fontSize = 16.sp,
            color = Color.Gray
        )

        Spacer(modifier = Modifier.height(32.dp))

        Card(
            modifier = Modifier.fillMaxWidth(),
            colors = CardDefaults.cardColors(
                containerColor = Color(0xFF2C2C2C)
            )
        ) {
            Column(modifier = Modifier.padding(20.dp)) {
                InfoRow(label = "開發者", value = "Your Name")
                Spacer(modifier = Modifier.height(12.dp))
                InfoRow(label = "更新日期", value = "2024/11/01")
                Spacer(modifier = Modifier.height(12.dp))
                InfoRow(label = "授權", value = "MIT License")
            }
        }

        Spacer(modifier = Modifier.height(24.dp))

        Text(
            text = "功能說明",
            fontSize = 18.sp,
            fontWeight = FontWeight.Bold,
            color = Color.White,
            modifier = Modifier.fillMaxWidth()
        )

        Spacer(modifier = Modifier.height(12.dp))

        Card(
            modifier = Modifier.fillMaxWidth(),
            colors = CardDefaults.cardColors(
                containerColor = Color(0xFF2C2C2C)
            )
        ) {
            Column(modifier = Modifier.padding(20.dp)) {
                Text(
                    text = "• 快速掃描 QR Code 和條碼",
                    color = Color.White,
                    fontSize = 14.sp
                )
                Spacer(modifier = Modifier.height(8.dp))
                Text(
                    text = "• 透過藍牙傳送至 STM32",
                    color = Color.White,
                    fontSize = 14.sp
                )
                Spacer(modifier = Modifier.height(8.dp))
                Text(
                    text = "• 在 E-Paper 顯示掃描內容",
                    color = Color.White,
                    fontSize = 14.sp
                )
                Spacer(modifier = Modifier.height(8.dp))
                Text(
                    text = "• 儲存掃描歷史記錄",
                    color = Color.White,
                    fontSize = 14.sp
                )
            }
        }

        Spacer(modifier = Modifier.weight(1f))

        Text(
            text = "© 2024 QR Scanner. All rights reserved.",
            fontSize = 12.sp,
            color = Color.Gray
        )
    }
}

@Composable
fun InfoRow(label: String, value: String) {
    Row(
        modifier = Modifier.fillMaxWidth(),
        horizontalArrangement = Arrangement.SpaceBetween
    ) {
        Text(
            text = label,
            color = Color.Gray,
            fontSize = 14.sp
        )
        Text(
            text = value,
            color = Color.White,
            fontSize = 14.sp,
            fontWeight = FontWeight.Medium
        )
    }
}

