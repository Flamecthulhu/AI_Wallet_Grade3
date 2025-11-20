#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_LINE 1024
#define MAX_SAMPLES 10000
#define INPUT_SIZE 9
#define HIDDEN1_SIZE 64
#define HIDDEN2_SIZE 32
#define OUTPUT_SIZE 5
#define LEARNING_RATE 0.001
#define FILE_PATH "Data\\training_data.csv"

typedef struct {
    float **w1, **w2, **w3;
    float *b1, *b2, *b3;
    float **dw1, **dw2, **dw3;
    float *db1, *db2, *db3;
    float **m_w1, **m_w2, **m_w3;
    float *m_b1, *m_b2, *m_b3;
    float **v_w1, **v_w2, **v_w3;
    float *v_b1, *v_b2, *v_b3;
} Network;

typedef struct {
    float *h1, *h2, *output;
    float *h1_activated, *h2_activated;
} ForwardCache;

float randn() {
    return ((float)rand() / RAND_MAX - 0.5f) * 0.2f;
}

float relu(float x) {
    return x > 0 ? x : 0;
}

float relu_derivative(float x) {
    return x > 0 ? 1 : 0;
}

void softmax(float *input, float *output, int size) {
    float max = input[0];
    for (int i = 1; i < size; i++) {
        if (input[i] > max) max = input[i];
    }
    float sum = 0;
    for (int i = 0; i < size; i++) {
        output[i] = expf(input[i] - max);
        sum += output[i];
    }
    for (int i = 0; i < size; i++) {
        output[i] /= sum;
    }
}

Network* create_network() {
    Network *net = (Network*)malloc(sizeof(Network));
    
    net->w1 = (float**)malloc(HIDDEN1_SIZE * sizeof(float*));
    net->w2 = (float**)malloc(HIDDEN2_SIZE * sizeof(float*));
    net->w3 = (float**)malloc(OUTPUT_SIZE * sizeof(float*));
    net->dw1 = (float**)malloc(HIDDEN1_SIZE * sizeof(float*));
    net->dw2 = (float**)malloc(HIDDEN2_SIZE * sizeof(float*));
    net->dw3 = (float**)malloc(OUTPUT_SIZE * sizeof(float*));
    net->m_w1 = (float**)malloc(HIDDEN1_SIZE * sizeof(float*));
    net->m_w2 = (float**)malloc(HIDDEN2_SIZE * sizeof(float*));
    net->m_w3 = (float**)malloc(OUTPUT_SIZE * sizeof(float*));
    net->v_w1 = (float**)malloc(HIDDEN1_SIZE * sizeof(float*));
    net->v_w2 = (float**)malloc(HIDDEN2_SIZE * sizeof(float*));
    net->v_w3 = (float**)malloc(OUTPUT_SIZE * sizeof(float*));
    
    for (int i = 0; i < HIDDEN1_SIZE; i++) {
        net->w1[i] = (float*)malloc(INPUT_SIZE * sizeof(float));
        net->dw1[i] = (float*)calloc(INPUT_SIZE, sizeof(float));
        net->m_w1[i] = (float*)calloc(INPUT_SIZE, sizeof(float));
        net->v_w1[i] = (float*)calloc(INPUT_SIZE, sizeof(float));
        for (int j = 0; j < INPUT_SIZE; j++) {
            net->w1[i][j] = randn();
        }
    }
    
    for (int i = 0; i < HIDDEN2_SIZE; i++) {
        net->w2[i] = (float*)malloc(HIDDEN1_SIZE * sizeof(float));
        net->dw2[i] = (float*)calloc(HIDDEN1_SIZE, sizeof(float));
        net->m_w2[i] = (float*)calloc(HIDDEN1_SIZE, sizeof(float));
        net->v_w2[i] = (float*)calloc(HIDDEN1_SIZE, sizeof(float));
        for (int j = 0; j < HIDDEN1_SIZE; j++) {
            net->w2[i][j] = randn();
        }
    }
    
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        net->w3[i] = (float*)malloc(HIDDEN2_SIZE * sizeof(float));
        net->dw3[i] = (float*)calloc(HIDDEN2_SIZE, sizeof(float));
        net->m_w3[i] = (float*)calloc(HIDDEN2_SIZE, sizeof(float));
        net->v_w3[i] = (float*)calloc(HIDDEN2_SIZE, sizeof(float));
        for (int j = 0; j < HIDDEN2_SIZE; j++) {
            net->w3[i][j] = randn();
        }
    }
    
    net->b1 = (float*)calloc(HIDDEN1_SIZE, sizeof(float));
    net->b2 = (float*)calloc(HIDDEN2_SIZE, sizeof(float));
    net->b3 = (float*)calloc(OUTPUT_SIZE, sizeof(float));
    net->db1 = (float*)calloc(HIDDEN1_SIZE, sizeof(float));
    net->db2 = (float*)calloc(HIDDEN2_SIZE, sizeof(float));
    net->db3 = (float*)calloc(OUTPUT_SIZE, sizeof(float));
    net->m_b1 = (float*)calloc(HIDDEN1_SIZE, sizeof(float));
    net->m_b2 = (float*)calloc(HIDDEN2_SIZE, sizeof(float));
    net->m_b3 = (float*)calloc(OUTPUT_SIZE, sizeof(float));
    net->v_b1 = (float*)calloc(HIDDEN1_SIZE, sizeof(float));
    net->v_b2 = (float*)calloc(HIDDEN2_SIZE, sizeof(float));
    net->v_b3 = (float*)calloc(OUTPUT_SIZE, sizeof(float));
    
    return net;
}

ForwardCache* forward(Network *net, float *input) {
    ForwardCache *cache = (ForwardCache*)malloc(sizeof(ForwardCache));
    cache->h1 = (float*)malloc(HIDDEN1_SIZE * sizeof(float));
    cache->h1_activated = (float*)malloc(HIDDEN1_SIZE * sizeof(float));
    cache->h2 = (float*)malloc(HIDDEN2_SIZE * sizeof(float));
    cache->h2_activated = (float*)malloc(HIDDEN2_SIZE * sizeof(float));
    cache->output = (float*)malloc(OUTPUT_SIZE * sizeof(float));
    
    for (int i = 0; i < HIDDEN1_SIZE; i++) {
        cache->h1[i] = net->b1[i];
        for (int j = 0; j < INPUT_SIZE; j++) {
            cache->h1[i] += net->w1[i][j] * input[j];
        }
        cache->h1_activated[i] = relu(cache->h1[i]);
    }
    
    for (int i = 0; i < HIDDEN2_SIZE; i++) {
        cache->h2[i] = net->b2[i];
        for (int j = 0; j < HIDDEN1_SIZE; j++) {
            cache->h2[i] += net->w2[i][j] * cache->h1_activated[j];
        }
        cache->h2_activated[i] = relu(cache->h2[i]);
    }
    
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        cache->output[i] = net->b3[i];
        for (int j = 0; j < HIDDEN2_SIZE; j++) {
            cache->output[i] += net->w3[i][j] * cache->h2_activated[j];
        }
    }
    
    return cache;
}

float cross_entropy_loss(float *output, int label) {
    float probs[OUTPUT_SIZE];
    softmax(output, probs, OUTPUT_SIZE);
    return -logf(probs[label] + 1e-10);
}

void backward(Network *net, float *input, ForwardCache *cache, int label, int batch_size) {
    float probs[OUTPUT_SIZE];
    softmax(cache->output, probs, OUTPUT_SIZE);
    
    float dout[OUTPUT_SIZE];
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        dout[i] = probs[i];
    }
    dout[label] -= 1;
    
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        for (int j = 0; j < HIDDEN2_SIZE; j++) {
            net->dw3[i][j] += dout[i] * cache->h2_activated[j] / batch_size;
        }
        net->db3[i] += dout[i] / batch_size;
    }
    
    float dh2[HIDDEN2_SIZE];
    for (int i = 0; i < HIDDEN2_SIZE; i++) {
        dh2[i] = 0;
        for (int j = 0; j < OUTPUT_SIZE; j++) {
            dh2[i] += dout[j] * net->w3[j][i];
        }
        dh2[i] *= relu_derivative(cache->h2[i]);
    }
    
    for (int i = 0; i < HIDDEN2_SIZE; i++) {
        for (int j = 0; j < HIDDEN1_SIZE; j++) {
            net->dw2[i][j] += dh2[i] * cache->h1_activated[j] / batch_size;
        }
        net->db2[i] += dh2[i] / batch_size;
    }
    
    float dh1[HIDDEN1_SIZE];
    for (int i = 0; i < HIDDEN1_SIZE; i++) {
        dh1[i] = 0;
        for (int j = 0; j < HIDDEN2_SIZE; j++) {
            dh1[i] += dh2[j] * net->w2[j][i];
        }
        dh1[i] *= relu_derivative(cache->h1[i]);
    }
    
    for (int i = 0; i < HIDDEN1_SIZE; i++) {
        for (int j = 0; j < INPUT_SIZE; j++) {
            net->dw1[i][j] += dh1[i] * input[j] / batch_size;
        }
        net->db1[i] += dh1[i] / batch_size;
    }
}

void adam_update(Network *net, int t) {
    float beta1 = 0.9, beta2 = 0.999, epsilon = 1e-8;
    float lr_t = LEARNING_RATE * sqrtf(1 - powf(beta2, t)) / (1 - powf(beta1, t));
    
    for (int i = 0; i < HIDDEN1_SIZE; i++) {
        for (int j = 0; j < INPUT_SIZE; j++) {
            net->m_w1[i][j] = beta1 * net->m_w1[i][j] + (1 - beta1) * net->dw1[i][j];
            net->v_w1[i][j] = beta2 * net->v_w1[i][j] + (1 - beta2) * net->dw1[i][j] * net->dw1[i][j];
            net->w1[i][j] -= lr_t * net->m_w1[i][j] / (sqrtf(net->v_w1[i][j]) + epsilon);
            net->dw1[i][j] = 0;
        }
        net->m_b1[i] = beta1 * net->m_b1[i] + (1 - beta1) * net->db1[i];
        net->v_b1[i] = beta2 * net->v_b1[i] + (1 - beta2) * net->db1[i] * net->db1[i];
        net->b1[i] -= lr_t * net->m_b1[i] / (sqrtf(net->v_b1[i]) + epsilon);
        net->db1[i] = 0;
    }
    
    for (int i = 0; i < HIDDEN2_SIZE; i++) {
        for (int j = 0; j < HIDDEN1_SIZE; j++) {
            net->m_w2[i][j] = beta1 * net->m_w2[i][j] + (1 - beta1) * net->dw2[i][j];
            net->v_w2[i][j] = beta2 * net->v_w2[i][j] + (1 - beta2) * net->dw2[i][j] * net->dw2[i][j];
            net->w2[i][j] -= lr_t * net->m_w2[i][j] / (sqrtf(net->v_w2[i][j]) + epsilon);
            net->dw2[i][j] = 0;
        }
        net->m_b2[i] = beta1 * net->m_b2[i] + (1 - beta1) * net->db2[i];
        net->v_b2[i] = beta2 * net->v_b2[i] + (1 - beta2) * net->db2[i] * net->db2[i];
        net->b2[i] -= lr_t * net->m_b2[i] / (sqrtf(net->v_b2[i]) + epsilon);
        net->db2[i] = 0;
    }
    
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        for (int j = 0; j < HIDDEN2_SIZE; j++) {
            net->m_w3[i][j] = beta1 * net->m_w3[i][j] + (1 - beta1) * net->dw3[i][j];
            net->v_w3[i][j] = beta2 * net->v_w3[i][j] + (1 - beta2) * net->dw3[i][j] * net->dw3[i][j];
            net->w3[i][j] -= lr_t * net->m_w3[i][j] / (sqrtf(net->v_w3[i][j]) + epsilon);
            net->dw3[i][j] = 0;
        }
        net->m_b3[i] = beta1 * net->m_b3[i] + (1 - beta1) * net->db3[i];
        net->v_b3[i] = beta2 * net->v_b3[i] + (1 - beta2) * net->db3[i] * net->db3[i];
        net->b3[i] -= lr_t * net->m_b3[i] / (sqrtf(net->v_b3[i]) + epsilon);
        net->db3[i] = 0;
    }
}

void print_network(Network *net) {
    int i, j;
    printf("\n--- FINAL NETWORK PARAMETERS ---\n");

    printf("\nB1 (biases hidden1):\n");
    for (i = 0; i < HIDDEN1_SIZE; i++) {
        printf("%.8f%s", net->b1[i], (i % 8 == 7) ? "\n" : " ");
    }
    if (HIDDEN1_SIZE % 8) printf("\n");

    printf("\nW1 (hidden1 x input):\n");
    for (i = 0; i < HIDDEN1_SIZE; i++) {
        for (j = 0; j < INPUT_SIZE; j++) {
            printf("%.8f ", net->w1[i][j]);
        }
        printf("\n");
    }

    printf("\nB2 (biases hidden2):\n");
    for (i = 0; i < HIDDEN2_SIZE; i++) {
        printf("%.8f%s", net->b2[i], (i % 8 == 7) ? "\n" : " ");
    }
    if (HIDDEN2_SIZE % 8) printf("\n");

    printf("\nW2 (hidden2 x hidden1):\n");
    for (i = 0; i < HIDDEN2_SIZE; i++) {
        for (j = 0; j < HIDDEN1_SIZE; j++) {
            printf("%.8f ", net->w2[i][j]);
        }
        printf("\n");
    }

    printf("\nB3 (biases output):\n");
    for (i = 0; i < OUTPUT_SIZE; i++) {
        printf("%.8f%s", net->b3[i], (i % 8 == 7) ? "\n" : " ");
    }
    if (OUTPUT_SIZE % 8) printf("\n");

    printf("\nW3 (output x hidden2):\n");
    for (i = 0; i < OUTPUT_SIZE; i++) {
        for (j = 0; j < HIDDEN2_SIZE; j++) {
            printf("%.8f ", net->w3[i][j]);
        }
        printf("\n");
    }

    printf("\n--- END PARAMETERS ---\n");
    fflush(stdout);
}

int main() {
    clock_t start_time = clock();
    srand(time(NULL));
    
    float inputs[MAX_SAMPLES][INPUT_SIZE];
    int labels[MAX_SAMPLES];
    int num_samples = 0;
    
    FILE *file = fopen("Data\\training_data.csv", "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }
    
    char line[MAX_LINE];
    fgets(line, sizeof(line), file);
    
    while (fgets(line, sizeof(line), file) && num_samples < MAX_SAMPLES) 
    {
        char *token = strtok(line, ",");
        for (int i = 0; i < INPUT_SIZE && token; i++) 
        {
            inputs[num_samples][i] = atof(token);
            token = strtok(NULL, ",");
        }

        if (token) 
        {
            labels[num_samples] = atoi(token);
            num_samples++;
        }
    }
    fclose(file);
    
    printf("Loaded %d samples\n", num_samples);
    
    Network *net = create_network();
    int adam_t = 0;
    
    for (int round = 0; round < 50; round++) {
        printf("Round %d\n", round + 1);
        
        for (int epoch = 0; epoch < 200; epoch++) {
            float total_loss = 0;
            
            for (int i = 0; i < num_samples; i++) {
                ForwardCache *cache = forward(net, inputs[i]);
                float loss = cross_entropy_loss(cache->output, labels[i]);
                total_loss += loss;
                
                backward(net, inputs[i], cache, labels[i], num_samples);
                
                free(cache->h1);
                free(cache->h1_activated);
                free(cache->h2);
                free(cache->h2_activated);
                free(cache->output);
                free(cache);
            }
            
            adam_t++;
            adam_update(net, adam_t);
            
            if (epoch % 100 == 0) {
                printf("Epoch %d, Loss: %.8f\n", epoch, total_loss / num_samples);
            }
        }
        
        float test_sample[INPUT_SIZE] = {25.014445,121.214801,22,1323,3,12,0,1,0};
        ForwardCache *cache = forward(net, test_sample);
        
        float probs[OUTPUT_SIZE];
        softmax(cache->output, probs, OUTPUT_SIZE);
        
        int predicted = 0;
        float max_prob = probs[0];
        for (int i = 1; i < OUTPUT_SIZE; i++) {
            if (probs[i] > max_prob) {
                max_prob = probs[i];
                predicted = i;
            }
        }
        
        printf("Predicted class index: %d\n", predicted);
        
        free(cache->h1);
        free(cache->h1_activated);
        free(cache->h2);
        free(cache->h2_activated);
        free(cache->output);
        free(cache);
    }
    
    clock_t end_time = clock();
    printf("Elapsed time: %.3f seconds\n", (double)(end_time - start_time) / CLOCKS_PER_SEC);

    // print final weights and biases
    print_network(net);

    system("pause");
    return 0;
}