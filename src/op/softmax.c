/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * License); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * AS IS BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*
 * Copyright (c) 2020, OPEN AI LAB
 * Author: haitao@openailab.com
 */

#include <stdio.h>
#include <assert.h>

#include "sys_port.h"
#include "tengine_ir.h"
#include "tengine_errno.h"
#include "tengine_log.h"
#include "tengine_op.h"
#include "parameter.h"
#include "softmax_param.h"

DEFINE_PARM_PARSE_ENTRY(softmax_param, axis);

static int infer_shape(struct ir_node* node)
{
    struct ir_graph* ir_graph = node->graph;
    struct ir_tensor* input = get_ir_graph_tensor(ir_graph, node->input_tensors[0]);
    struct ir_tensor* output = get_ir_graph_tensor(ir_graph, node->output_tensors[0]);

    output->layout = input->layout;

    set_ir_tensor_shape(output, input->dims, input->dim_num);

    return 0;
}

static int init_op(struct ir_op* op)
{
    struct softmax_param* softmax_param = ( struct softmax_param* )sys_malloc(sizeof(struct softmax_param));

    if (softmax_param == NULL)
    {
        set_tengine_errno(ENOMEM);
        return -1;
    }

    /*set the param default value */
    softmax_param->axis = 1;

    op->param_mem = softmax_param;
    op->param_size = sizeof(struct softmax_param);
    op->same_shape = 0;
    op->infer_shape = infer_shape;

    return 0;
}

static void release_op(struct ir_op* op)
{
    sys_free(op->param_mem);
}

static int register_softmax_op(void* arg)
{
    struct op_method m;

    m.op_version = 1;
    m.init_op = init_op;
    m.release_op = release_op;
    m.access_param_entry = access_param_entry;

    return register_op(OP_SOFTMAX, OP_SOFTMAX_NAME, &m);
}

static int unregister_softmax_op(void* arg)
{
    sys_free(GET_PARAM_PARSE_MAP(softmax_param));
    return unregister_op(OP_SOFTMAX, 1);
}

AUTO_REGISTER_OP(register_softmax_op);
AUTO_UNREGISTER_OP(unregister_softmax_op);