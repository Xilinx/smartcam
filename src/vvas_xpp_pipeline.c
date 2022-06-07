/*
 * Copyright 2021 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <vvas/vvas_kernel.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

enum
{
  LOG_LEVEL_ERROR,
  LOG_LEVEL_WARNING,
  LOG_LEVEL_INFO,
  LOG_LEVEL_DEBUG
};

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define LOG_MESSAGE(level, ...) {\
  do {\
    char *str; \
    if (level == LOG_LEVEL_ERROR)\
      str = (char*)"ERROR";\
    else if (level == LOG_LEVEL_WARNING)\
      str = (char*)"WARNING";\
    else if (level == LOG_LEVEL_INFO)\
      str = (char*)"INFO";\
    else if (level == LOG_LEVEL_DEBUG)\
      str = (char*)"DEBUG";\
    if (level <= kernel_priv->log_level) {\
      printf("[%s %s:%d] %s: ",__FILENAME__, __func__, __LINE__, str);\
      printf(__VA_ARGS__);\
      printf("\n");\
    }\
  } while (0); \
}

typedef struct _kern_priv
{
    float mean_r;
    float mean_g;
    float mean_b;
    float scale_r;
    float scale_g;
    float scale_b;
    VVASFrame *params;
    int log_level;
} ResizeKernelPriv;

int32_t
xlnx_kernel_start(VVASKernel *handle, int start, VVASFrame *input[MAX_NUM_OBJECT], VVASFrame *output[MAX_NUM_OBJECT]);
int32_t xlnx_kernel_done(VVASKernel *handle);
int32_t xlnx_kernel_init(VVASKernel *handle);
uint32_t xlnx_kernel_deinit(VVASKernel *handle);

uint32_t xlnx_kernel_deinit(VVASKernel *handle)
{
    ResizeKernelPriv *kernel_priv;
    kernel_priv = (ResizeKernelPriv *)handle->kernel_priv;
    vvas_free_buffer (handle, kernel_priv->params);
    free(kernel_priv);
    return 0;
}

int32_t xlnx_kernel_init(VVASKernel *handle)
{
    json_t *jconfig = handle->kernel_config;
    json_t *val; /* kernel config from app */
    ResizeKernelPriv *kernel_priv;
    float *pPtr; 

    handle->is_multiprocess = 0;
    kernel_priv = (ResizeKernelPriv *)calloc(1, sizeof(ResizeKernelPriv));
    if (!kernel_priv) {
        printf("Error: Unable to allocate resize kernel memory\n");
    }

    /* parse config */
    val = json_object_get(jconfig, "mean_r");
    if (!val || !json_is_number(val))
        kernel_priv->mean_r = 0;
    else {
        kernel_priv->mean_r = json_number_value(val);
    }
    printf("Resize: mean_r=%f\n", kernel_priv->mean_r);

    val = json_object_get(jconfig, "mean_g");
    if (!val || !json_is_number(val))
        kernel_priv->mean_g = 0;
    else {
        kernel_priv->mean_g = json_number_value(val);
    }
    printf("Resize: mean_g=%f\n", kernel_priv->mean_g);

    val = json_object_get(jconfig, "mean_b");
    if (!val || !json_is_number(val))
        kernel_priv->mean_b = 0;
    else {
        kernel_priv->mean_b = json_number_value(val);
    }
    printf("Resize: mean_b=%f\n", kernel_priv->mean_b);

    /* parse config */
    val = json_object_get(jconfig, "scale_r");
    if (!val || !json_is_number(val))
	kernel_priv->scale_r = 1;
    else
	kernel_priv->scale_r = json_number_value(val);
    printf("Resize: scale_r=%f\n", kernel_priv->scale_r);

    val = json_object_get(jconfig, "scale_g");
    if (!val || !json_is_number(val))
	kernel_priv->scale_g = 1;
    else
	kernel_priv->scale_g = json_number_value(val);
    printf("Resize: scale_g=%f\n", kernel_priv->scale_g);

    val = json_object_get(jconfig, "scale_b");
    if (!val || !json_is_number(val))
	kernel_priv->scale_b = 1;
    else
	kernel_priv->scale_b = json_number_value(val);
    printf("Resize: scale_b=%f\n", kernel_priv->scale_b);

    val = json_object_get (jconfig, "debug_level");
    if (!val || !json_is_integer (val))
        kernel_priv->log_level = LOG_LEVEL_WARNING;
    else
        kernel_priv->log_level = json_integer_value (val);



    kernel_priv->params = vvas_alloc_buffer (handle, 6*(sizeof(float)), VVAS_INTERNAL_MEMORY, DEFAULT_MEM_BANK, NULL);
    pPtr = kernel_priv->params->vaddr[0];
    pPtr[0] = (float)kernel_priv->mean_r;  
    pPtr[1] = (float)kernel_priv->mean_g;  
    pPtr[2] = (float)kernel_priv->mean_b;  
    pPtr[3] = (float)kernel_priv->scale_r;  
    pPtr[4] = (float)kernel_priv->scale_g;  
    pPtr[5] = (float)kernel_priv->scale_b;  

    handle->kernel_priv = (void *)kernel_priv;

    return 0;
}

int32_t xlnx_kernel_start(VVASKernel *handle, int start, VVASFrame *input[MAX_NUM_OBJECT], VVASFrame *output[MAX_NUM_OBJECT])
{
    ResizeKernelPriv *kernel_priv;
    kernel_priv = (ResizeKernelPriv *)handle->kernel_priv;

    int ret = vvas_kernel_start (handle, "ppppuuuuuu", 
        (input[0]->paddr[0]),
        (input[0]->paddr[1]),
        (output[0]->paddr[0]),
        (kernel_priv->params->paddr[0]),
        (input[0]->props.width),
        (input[0]->props.height),
        (input[0]->props.stride),
        (output[0]->props.width),
        (output[0]->props.height),
        (output[0]->props.width)
        );
    if (ret < 0) {
      LOG_MESSAGE (LOG_LEVEL_ERROR, "Preprocess: failed to issue execute command");
      return ret;
    }

    /* wait for kernel completion */
    ret = vvas_kernel_done (handle, 1000);
    if (ret < 0) {
      LOG_MESSAGE (LOG_LEVEL_ERROR, "Preprocess: failed to receive response from kernel");
      return ret;
    }

    return 0;
}

int32_t xlnx_kernel_done(VVASKernel *handle)
{
    return 0;
}
