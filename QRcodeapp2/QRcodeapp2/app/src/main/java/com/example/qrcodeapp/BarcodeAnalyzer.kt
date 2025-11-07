package com.example.qrcodeapp

import android.annotation.SuppressLint
import androidx.camera.core.ImageAnalysis
import androidx.camera.core.ImageProxy
import com.google.mlkit.vision.barcode.BarcodeScanning
import com.google.mlkit.vision.common.InputImage

class BarcodeAnalyzer(
    private val onBarcodeScanned: (String) -> Unit
) : ImageAnalysis.Analyzer {

    private val scanner = BarcodeScanning.getClient()

    @SuppressLint("UnsafeOptInUsageError")
    override fun analyze(imageProxy: ImageProxy) {
        val mediaImage = imageProxy.image
        if (mediaImage != null) {
            val image = InputImage.fromMediaImage(mediaImage, imageProxy.imageInfo.rotationDegrees)

            scanner.process(image)
                .addOnSuccessListener { barcodes ->
                    // 偵測到條碼
                    if (barcodes.isNotEmpty()) {
                        barcodes.firstOrNull()?.rawValue?.let { barcodeValue ->
                            onBarcodeScanned(barcodeValue)
                        }
                    }
                }
                .addOnFailureListener {
                    // 處理錯誤
                    it.printStackTrace()
                }
                .addOnCompleteListener {
                    // **非常重要**：分析完成後必須關閉 imageProxy
                    imageProxy.close()
                }
        }
    }
}