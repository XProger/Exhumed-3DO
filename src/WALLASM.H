void rectTransform(fix32 wx, fix32 wy, fix32 wz, sint32 light, sint32 h, sint32 w, fix32 px, fix32 py, fix32 pz, fix32 hx, fix32 hy, fix32 hz, void* output, uint16 (*lightFunc)(sint8 vLightIndex, MthXyz* pos));

void normTransform(sVertexType* firstVertex, MthMatrix* viewMatrix, sint32 nmVert, void* output, uint16 (*lightFunc)(sint8 vLightIndex, MthXyz* pos));
