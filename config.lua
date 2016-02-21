

local ffi = require('ffi')
ffi.cdef[[
struct qTestStruct { int a; int b; };
void qTest(struct qTestStruct* str);
]]

width = 1024
height = 768



str = ffi.new("struct qTestStruct", {a=1,b=2})
ffi.C.qTest(str)
