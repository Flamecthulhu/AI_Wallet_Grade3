#include <math.h>
#include <float.h>

#define N 60
#define INPUT_SIZE 8
#define K 3

float knn_predict(float X[N][INPUT_SIZE], float Y[N], float x_new[INPUT_SIZE]) {
    float dists[N];
    
    // 1. 計算距離
    for(int i=0;i<N;i++){
        dists[i] = 0;
        for(int j=0;j<INPUT_SIZE;j++){
            float diff = X[i][j] - x_new[j];
            dists[i] += diff * diff;
        }
        dists[i] = sqrtf(dists[i]);
    }
    
    // 2. 找 K 個最小距離
    int knn_idx[K];
    for(int k=0;k<K;k++){
        float min_d = FLT_MAX;
        int min_i = -1;
        for(int i=0;i<N;i++){
            // 避免重複選到同一個
            int used = 0;
            for(int t=0;t<k;t++) if(knn_idx[t]==i) used=1;
            if(!used && dists[i]<min_d){
                min_d = dists[i];
                min_i = i;
            }
        }
        knn_idx[k] = min_i;
    }
    
    // 3. 回傳 K 個標籤平均
    float y_pred = 0;
    for(int k=0;k<K;k++){
        y_pred += Y[knn_idx[k]];
    }
    y_pred /= K;
    return y_pred;
}
