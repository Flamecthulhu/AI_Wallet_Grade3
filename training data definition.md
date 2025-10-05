# MLP data definition

## 格式 Format
Current_GPS_Lat,Current_GPS_Lon,Hour_Of_Day,Min_Of_Day,Day_Of_Week,Time_To_Dept,Has_Paper_Ticket,Has_QR_Ticket,Has_Card,Is_Ticket_Reg,Is_Entering,Is_Exiting,Is_Checking_In,Is_Ticket_Scaned,Is_Near_TRA,Is_Near_HSR,Is_Near_MRT,Is_Near_School,Output_Action

| Name | Definition | Example |
| --- | --- | --- |
| Current_GPS_Lat | 當前緯度 (Latitude)，連續數值 | 24.073273 |
| Current_GPS_Lon | 當前經度 (Longitude)，連續數值 | 120.701243 |
| Hour_Of_Day | 當前時刻 (小時)，0~23 | 14 |
| Min_Of_Day | 當前時刻 (分鐘)，0~1439 | 870 |
| Day_Of_Week | 星期，1~7 (7=星期日, 1=星期一) | 2 |
| Time_To_Dept | 距離最近一班預定出發時間的剩餘分鐘數。-1:無關，0:有效/可能有關，正整數:有關 | 15 |
| Has_Paper_Ticket | 是否有紙本票，-1:無關，0:無，1:有 | 1 |
| Has_QR_Ticket | 是否有 QR Code 票，-1:無關，0:無，1:有 | 0 |
| Has_Card | 是否持有感應卡片，-1:無關，0:無，1:有 | 1 |
| Is_Ticket_Reg | QR票是否已登錄/準備，-1:無關，0:無，1:有 | 1 |
| Is_Entering | 是否準備進站/進入區域，-1:無關，0:否，1:是 | 1 |
| Is_Exiting | 是否準備離站/離開區域，-1:無關，0:否，1:是 | 0 |
| Is_Checking_In | 是否準備報到/簽到，-1:無關，0:否，1:是 | 1 |
| Is_Ticket_Scaned | 票券/卡片是否已掃描，-1:無關，0:未掃描，1:已掃描 | 0 |
| Is_Near_TRA | 是否靠近台鐵車站，0:否，1:是 | 1 |
| Is_Near_HSR | 是否靠近高鐵車站，0:否，1:是 | 0 |
| Is_Near_MRT | 是否靠近捷運車站，0:否，1:是 | 0 |
| Is_Near_School | 是否靠近學校，0:否，1:是 | 0 |
| Output_Action | 推論後執行動作，Conduct_MOS(0), Insulate_MOS(1), Scan_QR(2), Hide_QR(3), Do_Nothing(4) | 2 |

## 解釋 Notes
- 所有 0/1 欄位皆為布林值，0=否/無，1=是/有，-1=無關(不適用)。
- Output_Action 為分類標籤，代表錢包應執行的動作。
- 經緯度為浮點數，時間相關欄位為整數。