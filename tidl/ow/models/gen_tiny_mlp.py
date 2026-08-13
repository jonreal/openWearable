#!/usr/bin/env python3
# openWearable hello-world net: [1,16,1,1] -> Conv1x1(16->8) + bias -> ReLU -> [1,8,1,1].
# 1x1 conv == fully-connected; TIDL maps it to the MMA. Fixed weights -> known reference.
import numpy as np
import onnx
from onnx import helper, TensorProto, numpy_helper

np.random.seed(0)
Cin, Cout = 16, 8
W = (np.random.randn(Cout, Cin, 1, 1) * 0.3).astype(np.float32)
b = (np.random.randn(Cout) * 0.1).astype(np.float32)

X = helper.make_tensor_value_info("input", TensorProto.FLOAT, [1, Cin, 1, 1])
Y = helper.make_tensor_value_info("output", TensorProto.FLOAT, [1, Cout, 1, 1])
nodes = [
    helper.make_node("Conv", ["input", "W", "b"], ["c"], kernel_shape=[1, 1]),
    helper.make_node("Relu", ["c"], ["output"]),
]
graph = helper.make_graph(nodes, "ow_tiny", [X], [Y],
                          [numpy_helper.from_array(W, "W"), numpy_helper.from_array(b, "b")])
model = helper.make_model(graph, opset_imports=[helper.make_opsetid("", 11)])
model.ir_version = 7
from onnx import shape_inference
model = shape_inference.infer_shapes(model)   # give TIDL every tensor's dims
onnx.checker.check_model(model)
onnx.save(model, "/home/root/tiny_mlp.onnx")

# fixed input + float reference (what the C7x should reproduce up to int8 quant error)
x = ((np.arange(Cin).astype(np.float32) / Cin) - 0.5).reshape(1, Cin, 1, 1)
yref = np.maximum(0.0, (W.reshape(Cout, Cin) @ x.reshape(Cin)) + b)
np.save("/home/root/tiny_x.npy", x)
np.save("/home/root/tiny_yref.npy", yref)
np.set_printoptions(precision=4, suppress=True)
print("input  x =", x.reshape(-1))
print("FLOAT REF y =", yref)
print("saved /home/root/tiny_mlp.onnx")
