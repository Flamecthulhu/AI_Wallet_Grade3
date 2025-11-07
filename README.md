# AI Powered Wallet
### [中文版 Traditional Chinese version](./README_TC.md)
## Overview
A smart wallet that uses location services (GPS/WiFi) to recommend the best options for users.

By collecting location data, it can automatically display the appropriate QR code for entry or exit, eliminating the need for physical tickets and reducing the risk of losing or damaging them.

## Quick guidence
1. [Key knowledges](#key-knowledges)
2. [Example use case](#example-use-case)
3. [Component list](#component-list)
4. [Software list](#software-list)
5. [Graphics](#graphics)
6. [Imports](#imports)
7. [Formula](#formula)
8. [Relate links](#relate-links)
9. [Reference articles](#reference-articles)

## Key knowledges
- Multilayer Perceptron(MLP)
- K-Nearst Neighborhood(KNN)
- Euclidean distance
- WiFi enforce positioning using fingerprinting
- Mathematics
   - Eular number
   - Square root
   - Partial derivative
   - Summation
   - Array
   - Sparse CCE

## Example Use Case
Analyzing location data, when the user arrives at a train station, school, or the place user frequently come, the wallet checks whether a ticket has been saved.
If yes, it automatically displays the QR code for access.
If not, it connects MOSFET’s source and drain on the coil to allow the use of EasyCard.

## Component list
| Type | Component |  Communication | Applicate Description | Price(NTD) | Quantity | Unit |
| --- | --- | --- | --- | --- | --- | --- |
| Board | [ST STM32H7VIT6](https://shopee.tw/STM32H7%E6%A0%B8%E5%BF%83%E6%9D%BF-%E5%8E%9F%E8%A3%9DSTM32H750VBT6-STM32H743VIT6-%E9%96%8B%E7%99%BC%E6%9D%BF%E6%A0%B8%E5%BF%83%E6%9D%BF-i.1373963606.29520723765?xptdk=6c2fda6e-0860-48dc-9459-c875ffafa929) | 4xUART, 4xI2C, 6xSPI |Calculate, Control | 652 | 1 | piece |
| Module | ublox Neo-7m | UART | Positioning, Time acquisition | 0 | 1 | unit |
| Module | [HM-10](https://shopee.tw/%E3%80%90%E5%8F%AF%E9%96%8B%E7%B5%B1%E7%B7%A8%E7%99%BC%E7%A5%A8%E3%80%91HM-10-%E9%80%8F%E6%98%8E%E4%B8%B2%E5%8F%A3-%E8%97%8D%E7%89%994.0%E6%A8%A1%E5%A1%8A-%E8%97%8D%E7%89%99%E4%B8%B2%E5%8F%A3-%E5%B8%B6%E9%82%8F%E8%BC%AF%E9%9B%BB%E5%B9%B3%E8%BD%89%E6%8F%9B-%E9%98%B2%E5%8F%8D-%E9%85%8D%E4%BB%B6-i.1255950178.28158604649) | UART | Communication, Data Transfer | 168 | 1 | unit |
| Module | [Espressif ESP-01S](https://shopee.tw/%E3%80%90%E5%8F%AF%E9%96%8B%E7%B5%B1%E7%B7%A8%E7%99%BC%E7%A5%A8%E3%80%91ESP8266%E4%B8%B2%E5%8F%A3WIFI-%E7%84%A1%E7%B7%9A%E6%A8%A1%E5%A1%8A-WIF%E6%94%B6%E7%99%BC%E7%84%A1%E7%B7%9A%E6%A8%A1%E5%A1%8A-ESP-01-ESP-01S-i.1255950178.26558242485) | UART | Increase positioning precision | 57 | 1 | unit |
| Module | [2.66' E-paper](https://shopee.tw/%E2%99%9E-%E2%99%98-%E2%99%992.66%E5%AF%B8%E9%9B%BB%E5%AD%90%E7%B4%99%E5%A2%A8%E6%B0%B4%E8%9E%A2%E5%B9%952.66%E5%AF%B8%E9%A1%AF%E7%A4%BA%E5%B1%8F%E9%BB%91%E7%99%BDEPD%E9%9B%BB%E5%AD%90%E7%B4%99%E9%A1%AF%E7%A4%BA%E5%B1%8F%E5%A2%A8%E6%B0%B4%E5%B1%8F-i.192829893.28031593160) | SPI | Showing informations | 371 | 1 | piece |
| Module | [TP4056](https://shopee.tw/%E3%80%90%E9%96%8B%E7%B5%B1%E7%B7%A8%E7%99%BC%E7%A5%A8%E3%80%91TP4056-1A-18650%E9%8B%B0%E9%9B%BB%E6%B1%A0%E5%85%85%E9%9B%BB%E4%BF%9D%E8%AD%B7%E6%9D%BF%E6%A8%A1%E5%A1%8A-Tpey-C%E6%AF%8D%E5%BA%A7-3.7V%E9%81%8E%E5%85%85-%E9%81%8E%E6%94%BE-i.1255950178.27211695884) |  | Charge battery | 11 | 1 | unit |
| Module | [MP1584EN](https://shopee.tw/%E3%80%90%E7%8F%BE%E8%B2%A8%E3%80%91-MP1584EN-%E9%99%8D%E5%A3%93%E6%A8%A1%E7%B5%84-DC-DC-3A-%E9%9B%BB%E6%BA%90%E6%A8%A1%E7%B5%84-%E5%8F%AF%E8%AA%BF-%E5%9B%BA%E5%AE%9A%E8%BC%B8%E5%87%BA-%E8%B6%85%E5%B0%8F%E9%AB%94%E7%A9%8D-%E8%BC%B8%E5%85%A54.5~28V-%E5%B0%8F%E9%BD%8A%E7%9A%84%E5%AE%B6-i.1536798550.42607613267?sp_atk=337dce50-0e61-4b2e-a480-89e0ecfe375d&xptdk=337dce50-0e61-4b2e-a480-89e0ecfe375d) |  | Voltage reduction | ~~45~~ | 1 | unit |
| IC | [EasyCard Chip](https://shopee.tw/%E5%B7%B2%E7%84%8A%E6%8E%A5%E5%A5%BD%E7%9A%84%E7%B7%9A%E5%9C%88-%E6%94%B9%E9%80%A0%E6%82%A0%E9%81%8A%E5%8D%A1-%E4%B8%80%E5%8D%A1%E9%80%9A-icash2.0-%E5%B0%88%E7%94%A8-%E5%8F%AF%E8%AD%B7%E8%B2%9D-%E8%B3%BC%E8%B2%B7%E5%89%8D%E8%AB%8B%E7%9C%8B%E5%95%86%E5%93%81%E6%8F%8F%E8%BF%B0-i.34448402.2671862034?sp_atk=80168b58-c73f-45b1-8f94-b450b0201075&xptdk=80168b58-c73f-45b1-8f94-b450b0201075) |  | Transit authentication | 199 | 1 | piece |
| IC | [RECOY R2-1A05](https://www.tme.eu/en/details/r2-1a05/reed-electromagnetic-relays/recoy-rayex-electronics/) |  | Relay, Coil switch | 40 | 1 | piece |
| Battery | ~~9000mAh~~ |  | Power conponents | x | 1 | pack |
| Total | | 3xUART, 1xI2C |  |  | ~~10~~ 9 | NTD |

## Software list
| Type | Name | Applicate Description | Example Usage |
| --- | --- | --- | --- |
| Application | Visual Studio Code | Coding programs |  |
| Application | STM32 CubeIDE | Coding MCU programs |  |
| Application | Autodesk Fusion | Design 3D model |  |
| Website | [Google](https://www.google.com) | Search data |  |
| Website | [Google Docs](https://www.docs.google.com) | Text presentation |  |
| Website | [GitHub](https://github.com) | Repository data, codes |  |
| Language | Python | Model training | [mlp_training.py](/mlp_training.py) |
| Language | TeX | Mathematics presentation | [mlp_steps.tex](/mlp_steps.tex) |
| Language | Markdown | Gather informations | [README.md](/README.md) |
| Language | C | Main MCU programming language | []() |
| Language | Kotlin | Android App develope | []() |
| Language | Swift | iOS App develope | []() |
| Language | Batch | Quick update to GitHub (Windows) | [auto update.cmd](/Basic/tools/auto%20update.cmd) |
| Language | Bash | Quick update to GitHub (Unix) | [auto update.sh](/Basic/tools/auto%20update.sh) |

## Graphics
![Multilayer Perceptron](/Data/pictures/mlp.png "MLP Image")
![K-Nearest Neighborhood](/Data/pictures/knn.png "KNN Image")

## Imports
```python
python3 -m pip install sympy

import sympy as sy
import math
import random
import csv
```

## Formula
- [MLP Steps (PDF)](./Data/mlp%20steps.pdf)
- [KNN Stpes (PDF)](./Data/knn%20steps.pdf)

## Relate Links
- [專題製作 (Google Docs)](https://docs.google.com/document/d/1-IISGgF8X5tYGCxTpJfdwXhCcVmH8Mb71v42PAtXhI0/edit?tab=t.0)
- [專題構想書 (Google Docs)](https://docs.google.com/document/d/1wJuGpTwVF7ZDf-fiy_uiF1CF_Y2mxeQgy5cqK6Bc2YU/edit?tab=t.0)
- [專題檔案 (Google Drive)](https://drive.google.com/drive/u/0/folders/1YwunItaLr3p2QV96NU1orU-EpbJaYyqv)


## Reference articles
### Positioning
- [On Outdoor Positioning with Wi-Fi and GPS-Lao-Sheng Lin, Ching-Chih Kuo](https://landeconomics.nccu.edu.tw/upload/29/download_file/5146/18-01-1,%E6%95%B4%E5%90%88Wi-Fi%E8%88%87GPS%E6%8A%80%E8%A1%93%E6%96%BC%E5%AE%A4%E5%A4%96%E5%AE%9A%E4%BD%8D%E4%B9%8B%E7%A0%94%E7%A9%B6_%E6%9E%97%E8%80%81%E7%94%9F,%E9%83%AD%E6%B8%85%E6%99%BA.pdf)
- [On outdoor positioning with Wi-Fi-Binghao Li, Ishrat J. Quader, Andrew G. Dempster](https://www.researchgate.net/publication/251404219_On_outdoor_positioning_with_Wi-Fi#fullTextFileContent)
### Machine Learning
- [Learning representations by back-propagating errors-David E. Rumelhari, Geoffrey E. Hintont & Ronald J. Williams-1986](https://gwern.net/doc/ai/nn/1986-rumelhart-2.pdf)

