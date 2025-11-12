#include <Arduino.h>

#define INPUT_SIZE 18
#define HIDDEN1_SIZE 8
#define HIDDEN2_SIZE 4
#define OUTPUT_SIZE 3
#define LEARNING_RATE 0.001
#define MAX_SAMPLES 10  // Reduced for Arduino memory constraints

typedef struct {
    float w1[HIDDEN1_SIZE][INPUT_SIZE];
    float w2[HIDDEN2_SIZE][HIDDEN1_SIZE];
    float w3[OUTPUT_SIZE][HIDDEN2_SIZE];
    float b1[HIDDEN1_SIZE];
    float b2[HIDDEN2_SIZE];
    float b3[OUTPUT_SIZE];
    float dw1[HIDDEN1_SIZE][INPUT_SIZE];
    float dw2[HIDDEN2_SIZE][HIDDEN1_SIZE];
    float dw3[OUTPUT_SIZE][HIDDEN2_SIZE];
    float db1[HIDDEN1_SIZE];
    float db2[HIDDEN2_SIZE];
    float db3[OUTPUT_SIZE];
    float m_w1[HIDDEN1_SIZE][INPUT_SIZE];
    float m_w2[HIDDEN2_SIZE][HIDDEN1_SIZE];
    float m_w3[OUTPUT_SIZE][HIDDEN2_SIZE];
    float m_b1[HIDDEN1_SIZE];
    float m_b2[HIDDEN2_SIZE];
    float m_b3[OUTPUT_SIZE];
    float v_w1[HIDDEN1_SIZE][INPUT_SIZE];
    float v_w2[HIDDEN2_SIZE][HIDDEN1_SIZE];
    float v_w3[OUTPUT_SIZE][HIDDEN2_SIZE];
    float v_b1[HIDDEN1_SIZE];
    float v_b2[HIDDEN2_SIZE];
    float v_b3[OUTPUT_SIZE];
} Network;

typedef struct {
    float h1[HIDDEN1_SIZE];
    float h1_activated[HIDDEN1_SIZE];
    float h2[HIDDEN2_SIZE];
    float h2_activated[HIDDEN2_SIZE];
    float output[OUTPUT_SIZE];
} ForwardCache;

// Global variables
Network net;
float inputs[MAX_SAMPLES][INPUT_SIZE];
int labels[MAX_SAMPLES];
int num_samples = 0;

float randn() {
    return ((float)random(10000) / 10000.0 - 0.5) * 0.2;
}

float relu(float x) {
    return x > 0 ? x : 0;
}

float relu_derivative(float x) {
    return x > 0 ? 1 : 0;
}

void softmax(float *input, float *output, int size) {
    float max_val = input[0];
    for (int i = 1; i < size; i++) {
        if (input[i] > max_val) max_val = input[i];
    }
    float sum = 0;
    for (int i = 0; i < size; i++) {
        output[i] = exp(input[i] - max_val);
        sum += output[i];
    }
    for (int i = 0; i < size; i++) {
        output[i] /= sum;
    }
}

void init_network() {
    // Initialize weights with random values
    for (int i = 0; i < HIDDEN1_SIZE; i++) {
        for (int j = 0; j < INPUT_SIZE; j++) {
            net.w1[i][j] = randn();
            net.dw1[i][j] = 0;
            net.m_w1[i][j] = 0;
            net.v_w1[i][j] = 0;
        }
        net.b1[i] = 0;
        net.db1[i] = 0;
        net.m_b1[i] = 0;
        net.v_b1[i] = 0;
    }
    
    for (int i = 0; i < HIDDEN2_SIZE; i++) {
        for (int j = 0; j < HIDDEN1_SIZE; j++) {
            net.w2[i][j] = randn();
            net.dw2[i][j] = 0;
            net.m_w2[i][j] = 0;
            net.v_w2[i][j] = 0;
        }
        net.b2[i] = 0;
        net.db2[i] = 0;
        net.m_b2[i] = 0;
        net.v_b2[i] = 0;
    }
    
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        for (int j = 0; j < HIDDEN2_SIZE; j++) {
            net.w3[i][j] = randn();
            net.dw3[i][j] = 0;
            net.m_w3[i][j] = 0;
            net.v_w3[i][j] = 0;
        }
        net.b3[i] = 0;
        net.db3[i] = 0;
        net.m_b3[i] = 0;
        net.v_b3[i] = 0;
    }
}

void forward(float *input, ForwardCache *cache) {
    // Layer 1
    for (int i = 0; i < HIDDEN1_SIZE; i++) {
        cache->h1[i] = net.b1[i];
        for (int j = 0; j < INPUT_SIZE; j++) {
            cache->h1[i] += net.w1[i][j] * input[j];
        }
        cache->h1_activated[i] = relu(cache->h1[i]);
    }
    
    // Layer 2
    for (int i = 0; i < HIDDEN2_SIZE; i++) {
        cache->h2[i] = net.b2[i];
        for (int j = 0; j < HIDDEN1_SIZE; j++) {
            cache->h2[i] += net.w2[i][j] * cache->h1_activated[j];
        }
        cache->h2_activated[i] = relu(cache->h2[i]);
    }
    
    // Layer 3
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        cache->output[i] = net.b3[i];
        for (int j = 0; j < HIDDEN2_SIZE; j++) {
            cache->output[i] += net.w3[i][j] * cache->h2_activated[j];
        }
    }
}

float cross_entropy_loss(float *output, int label) {
    float probs[OUTPUT_SIZE];
    softmax(output, probs, OUTPUT_SIZE);
    return -log(probs[label] + 1e-10);
}

void backward(float *input, ForwardCache *cache, int label, int batch_size) {
    float probs[OUTPUT_SIZE];
    softmax(cache->output, probs, OUTPUT_SIZE);
    
    float dout[OUTPUT_SIZE];
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        dout[i] = probs[i];
    }
    dout[label] -= 1;
    
    // Gradient for layer 3
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        for (int j = 0; j < HIDDEN2_SIZE; j++) {
            net.dw3[i][j] += dout[i] * cache->h2_activated[j] / batch_size;
        }
        net.db3[i] += dout[i] / batch_size;
    }
    
    // Backprop to layer 2
    float dh2[HIDDEN2_SIZE];
    for (int i = 0; i < HIDDEN2_SIZE; i++) {
        dh2[i] = 0;
        for (int j = 0; j < OUTPUT_SIZE; j++) {
            dh2[i] += dout[j] * net.w3[j][i];
        }
        dh2[i] *= relu_derivative(cache->h2[i]);
    }
    
    for (int i = 0; i < HIDDEN2_SIZE; i++) {
        for (int j = 0; j < HIDDEN1_SIZE; j++) {
            net.dw2[i][j] += dh2[i] * cache->h1_activated[j] / batch_size;
        }
        net.db2[i] += dh2[i] / batch_size;
    }
    
    // Backprop to layer 1
    float dh1[HIDDEN1_SIZE];
    for (int i = 0; i < HIDDEN1_SIZE; i++) {
        dh1[i] = 0;
        for (int j = 0; j < HIDDEN2_SIZE; j++) {
            dh1[i] += dh2[j] * net.w2[j][i];
        }
        dh1[i] *= relu_derivative(cache->h1[i]);
    }
    
    for (int i = 0; i < HIDDEN1_SIZE; i++) {
        for (int j = 0; j < INPUT_SIZE; j++) {
            net.dw1[i][j] += dh1[i] * input[j] / batch_size;
        }
        net.db1[i] += dh1[i] / batch_size;
    }
}

void adam_update(int t) {
    float beta1 = 0.9, beta2 = 0.999, epsilon = 1e-8;
    float lr_t = LEARNING_RATE * sqrt(1 - pow(beta2, t)) / (1 - pow(beta1, t));
    
    for (int i = 0; i < HIDDEN1_SIZE; i++) {
        for (int j = 0; j < INPUT_SIZE; j++) {
            net.m_w1[i][j] = beta1 * net.m_w1[i][j] + (1 - beta1) * net.dw1[i][j];
            net.v_w1[i][j] = beta2 * net.v_w1[i][j] + (1 - beta2) * net.dw1[i][j] * net.dw1[i][j];
            net.w1[i][j] -= lr_t * net.m_w1[i][j] / (sqrt(net.v_w1[i][j]) + epsilon);
            net.dw1[i][j] = 0;
        }
        net.m_b1[i] = beta1 * net.m_b1[i] + (1 - beta1) * net.db1[i];
        net.v_b1[i] = beta2 * net.v_b1[i] + (1 - beta2) * net.db1[i] * net.db1[i];
        net.b1[i] -= lr_t * net.m_b1[i] / (sqrt(net.v_b1[i]) + epsilon);
        net.db1[i] = 0;
    }
    
    for (int i = 0; i < HIDDEN2_SIZE; i++) {
        for (int j = 0; j < HIDDEN1_SIZE; j++) {
            net.m_w2[i][j] = beta1 * net.m_w2[i][j] + (1 - beta1) * net.dw2[i][j];
            net.v_w2[i][j] = beta2 * net.v_w2[i][j] + (1 - beta2) * net.dw2[i][j] * net.dw2[i][j];
            net.w2[i][j] -= lr_t * net.m_w2[i][j] / (sqrt(net.v_w2[i][j]) + epsilon);
            net.dw2[i][j] = 0;
        }
        net.m_b2[i] = beta1 * net.m_b2[i] + (1 - beta1) * net.db2[i];
        net.v_b2[i] = beta2 * net.v_b2[i] + (1 - beta2) * net.db2[i] * net.db2[i];
        net.b2[i] -= lr_t * net.m_b2[i] / (sqrt(net.v_b2[i]) + epsilon);
        net.db2[i] = 0;
    }
    
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        for (int j = 0; j < HIDDEN2_SIZE; j++) {
            net.m_w3[i][j] = beta1 * net.m_w3[i][j] + (1 - beta1) * net.dw3[i][j];
            net.v_w3[i][j] = beta2 * net.v_w3[i][j] + (1 - beta2) * net.dw3[i][j] * net.dw3[i][j];
            net.w3[i][j] -= lr_t * net.m_w3[i][j] / (sqrt(net.v_w3[i][j]) + epsilon);
            net.dw3[i][j] = 0;
        }
        net.m_b3[i] = beta1 * net.m_b3[i] + (1 - beta1) * net.db3[i];
        net.v_b3[i] = beta2 * net.v_b3[i] + (1 - beta2) * net.db3[i] * net.db3[i];
        net.b3[i] -= lr_t * net.m_b3[i] / (sqrt(net.v_b3[i]) + epsilon);
        net.db3[i] = 0;
    }
}

void load_hardcoded_data() {
    num_samples = 32;
    
    // Sample 0
    float s0[INPUT_SIZE] = {24.073273, 120.701243, 7, 473, 5, -1, -1, -1, 1, -1, 1, 0, 1, -1, 0, 0, 0, 1};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[0][i] = s0[i];
    labels[0] = 0;
    
    // Sample 1
    float s1[INPUT_SIZE] = {24.073273, 120.701243, 7, 475, 5, -1, -1, -1, 1, -1, 0, 0, 0, -1, 0, 0, 0, 1};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[1][i] = s1[i];
    labels[1] = 1;
    
    // Sample 2
    float s2[INPUT_SIZE] = {24.137501, 120.686928, 15, 944, 5, -1, -1, -1, 1, -1, 1, 0, -1, 0, 1, 0, 0, 0};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[2][i] = s2[i];
    labels[2] = 0;
    
    // Sample 3
    float s3[INPUT_SIZE] = {24.118987, 120.647883, 19, 1150, 2, 6, 0, 0, 1, 0, 1, 0, -1, 0, 1, 0, 1, 0};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[3][i] = s3[i];
    labels[3] = 0;
    
    // Sample 4
    float s4[INPUT_SIZE] = {24.171351, 120.660234, 9, 564, 4, 3, 0, 0, 1, 0, 0, 1, -1, 0, 0, 0, 1, 1};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[4][i] = s4[i];
    labels[4] = 0;
    
    // Sample 5
    float s5[INPUT_SIZE] = {24.109922, 120.614912, 7, 440, 5, 20, 1, 0, 0, 0, 1, 0, -1, 0, 1, 1, 1, 0};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[5][i] = s5[i];
    labels[5] = 2;
    
    // Sample 6
    float s6[INPUT_SIZE] = {25.051813, 121.60453, 8, 528, 5, 2, 1, 0, 0, 0, 0, 1, -1, 0, 1, 1, 1, 0};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[6][i] = s6[i];
    labels[6] = 2;
    
    // Sample 7
    float s7[INPUT_SIZE] = {25.014445, 121.214801, 22, 1323, 3, 12, 1, 0, 0, 0, 1, 0, -1, 0, 0, 1, 1, 0};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[7][i] = s7[i];
    labels[7] = 2;
    
    // Sample 8
    float s8[INPUT_SIZE] = {24.118944, 120.616505, 22, 1365, 3, 2, 1, 0, 0, 0, 0, 1, -1, 0, 1, 1, 1, 0};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[8][i] = s8[i];
    labels[8] = 2;
    
    // Sample 9
    float s9[INPUT_SIZE] = {25.013425, 121.214136, 22, 1332, 7, 3, 1, 1, 0, 1, 1, 0, -1, 0, 0, 1, 1, 0};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[9][i] = s9[i];
    labels[9] = 3;
    
    // Sample 10
    float s10[INPUT_SIZE] = {25.013425, 121.214136, 22, 1337, 7, 40, 1, 1, 0, 1, 0, 0, -1, 1, 0, 1, 1, 0};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[10][i] = s10[i];
    labels[10] = 4;
    
    // Sample 11
    float s11[INPUT_SIZE] = {24.112048, 120.615803, 22, 1365, 7, 2, 1, 1, 0, 1, 0, 1, -1, 0, 1, 1, 1, 0};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[11][i] = s11[i];
    labels[11] = 3;
    
    // Sample 12
    float s12[INPUT_SIZE] = {24.111614, 120.615578, 22, 1320, 7, -1, 1, 1, 0, 1, 0, 0, -1, 1, 1, 1, 1, 0};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[12][i] = s12[i];
    labels[12] = 4;
    
    // Sample 13
    float s13[INPUT_SIZE] = {25.013974, 121.463682, 18, 1129, 2, 10, 1, 1, 0, 1, 1, 0, -1, 0, 1, 1, 1, 0};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[13][i] = s13[i];
    labels[13] = 3;
    
    // Sample 14
    float s14[INPUT_SIZE] = {25.013974, 121.463682, 19, 1140, 2, 37, 1, 1, 0, 1, 0, 0, -1, 1, 1, 1, 1, 0};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[14][i] = s14[i];
    labels[14] = 4;
    
    // Sample 15
    float s15[INPUT_SIZE] = {24.110647, 120.615556, 21, 1290, 2, 9, 1, 0, 0, 0, 1, 0, -1, 0, 1, 1, 1, 0};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[15][i] = s15[i];
    labels[15] = 3;
    
    // Sample 16
    float s16[INPUT_SIZE] = {24.110647, 120.615556, 21, 1301, 2, 2, 1, 0, 0, 0, 1, 0, -1, 0, 1, 1, 1, 0};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[16][i] = s16[i];
    labels[16] = 3;
    
    // Sample 17
    float s17[INPUT_SIZE] = {24.073471, 120.69979, 7, 478, 2, -1, -1, -1, 1, -1, 1, 0, 1, 0, 0, 0, 0, 1};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[17][i] = s17[i];
    labels[17] = 0;
    
    // Sample 18
    float s18[INPUT_SIZE] = {24.073471, 120.69979, 7, 479, 2, -1, -1, -1, 1, -1, 0, 0, 0, -1, 0, 0, 0, 1};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[18][i] = s18[i];
    labels[18] = 1;
    
    // Sample 19
    float s19[INPUT_SIZE] = {24.109829, 120.614157, 21, 1311, 2, 5, 1, 1, 0, 1, 1, 0, -1, 0, 1, 1, 1, 0};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[19][i] = s19[i];
    labels[19] = 3;
    
    // Sample 20
    float s20[INPUT_SIZE] = {24.109829, 120.614157, 21, 1314, 2, 2, 1, 1, 0, 1, 0, 0, -1, 1, 1, 1, 1, 0};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[20][i] = s20[i];
    labels[20] = 4;
    
    // Sample 21
    float s21[INPUT_SIZE] = {24.137293, 120.686929, 6, 385, 1, 6, 1, 1, 0, 1, 1, 0, -1, 1, 1, 0, 0, 0};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[21][i] = s21[i];
    labels[21] = 3;
    
    // Sample 22
    float s22[INPUT_SIZE] = {24.137293, 120.686929, 7, 446, 1, 2, 1, 1, 0, 1, 0, 1, -1, 0, 1, 0, 0, 0};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[22][i] = s22[i];
    labels[22] = 3;
    
    // Sample 23
    float s23[INPUT_SIZE] = {24.805742, 121.039533, 1, 119, 4, 5, 1, 1, 0, 1, 1, 0, -1, 0, 1, 0, 0, 0};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[23][i] = s23[i];
    labels[23] = 3;
    
    // Sample 24
    float s24[INPUT_SIZE] = {24.073337, 120.699875, 7, 443, 3, -1, -1, -1, 1, -1, 1, 0, 1, -1, 0, 0, 0, 1};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[24][i] = s24[i];
    labels[24] = 0;
    
    // Sample 25
    float s25[INPUT_SIZE] = {24.072946, 120.699617, 7, 477, 2, -1, -1, -1, 1, -1, 1, 0, 1, -1, 0, 0, 0, 1};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[25][i] = s25[i];
    labels[25] = 0;
    
    // Sample 26
    float s26[INPUT_SIZE] = {24.137541, 120.686484, 17, 1053, 4, -1, -1, -1, 1, -1, 0, 0, -1, -1, 1, 0, 0, 0};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[26][i] = s26[i];
    labels[26] = 2;
    
    // Sample 27
    float s27[INPUT_SIZE] = {23.959449, 120.569682, 21, 1290, 4, 5, 1, 1, 0, 1, 1, 0, -1, 1, 1, 0, 0, 0};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[27][i] = s27[i];
    labels[27] = 4;
    
    // Sample 28
    float s28[INPUT_SIZE] = {24.800636, 120.970511, 7, 446, 1, 2, 1, 1, 0, 1, 0, 1, -1, 0, 1, 0, 0, 0};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[28][i] = s28[i];
    labels[28] = 3;
    
    // Sample 29
    float s29[INPUT_SIZE] = {24.988817, 121.314357, 21, 1288, 5, 2, 1, 1, 0, 1, 1, 0, -1, 0, 1, 0, 0, 0};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[29][i] = s29[i];
    labels[29] = 3;
    
    // Sample 30
    float s30[INPUT_SIZE] = {24.137148, 120.686742, 18, 1096, 1, 3, 1, 1, 0, 1, 1, 0, -1, 1, 1, 0, 0, 0};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[30][i] = s30[i];
    labels[30] = 3;
    
    // Sample 31
    float s31[INPUT_SIZE] = {24.073544, 120.699775, 7, 457, 5, -1, -1, -1, 1, -1, 1, 0, 1, -1, 0, 0, 0, 1};
    for (int i = 0; i < INPUT_SIZE; i++) inputs[31][i] = s31[i];
    labels[31] = 0;
    
    Serial.print("Loaded ");
    Serial.print(num_samples);
    Serial.println(" samples");
}

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ; // Wait for serial port to connect
    }
    
    Serial.println("Starting Neural Network Training on Arduino");
    unsigned long start_time = millis();
    
    randomSeed(analogRead(0)); // Seed random number generator
    
    // Load training data
    load_hardcoded_data();
    
    // Initialize network
    init_network();
    
    int adam_t = 0;
    
    // Training loop - reduced iterations for Arduino
    for (int round = 0; round < 10; round++) {
        Serial.print("Round ");
        Serial.println(round + 1);
        
        for (int epoch = 0; epoch < 50; epoch++) {
            float total_loss = 0;
            ForwardCache cache;
            
            for (int i = 0; i < num_samples; i++) {
                forward(inputs[i], &cache);
                float loss = cross_entropy_loss(cache.output, labels[i]);
                total_loss += loss;
                
                backward(inputs[i], &cache, labels[i], num_samples);
            }
            
            adam_t++;
            adam_update(adam_t);
            
            if (epoch % 25 == 0) {
                Serial.print("Epoch ");
                Serial.print(epoch);
                Serial.print(", Loss: ");
                Serial.println(total_loss / num_samples, 8);
            }
        }
    }
    
    // Test prediction
    float test_sample[INPUT_SIZE] = {24.073048, 120.699825, 7, 478, 2, -1, -1, -1, 1, -1, 1, 0, 1, -1, 0, 0, 0, 1};
    ForwardCache cache;
    forward(test_sample, &cache);
    
    float probs[OUTPUT_SIZE];
    softmax(cache.output, probs, OUTPUT_SIZE);
    
    int predicted = 0;
    float max_prob = probs[0];
    for (int i = 1; i < OUTPUT_SIZE; i++) {
        if (probs[i] > max_prob) {
            max_prob = probs[i];
            predicted = i;
        }
    }
    
    Serial.print("Predicted class index: ");
    Serial.println(predicted);
    
    unsigned long end_time = millis();
    Serial.print("Elapsed time: ");
    Serial.print((end_time - start_time) / 1000.0);
    Serial.println(" seconds");
    
    Serial.println("Training complete!");
}

void loop() {
    // Nothing to do in loop
    delay(1000);
}