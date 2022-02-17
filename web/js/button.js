var a = 0;
async function button() {
   a++;
   console.log(Module._Hello());
   console.log(Module._Sub(0, a));
   console.log(Module._Multiply(1, a));
   var ptr = Module._GetString();
   var str = UTF8ToString(ptr);
   console.log(typeof(str));
   console.log(str);
   ptr = allocateUTF8OnStack("你好，Emscripten！");
   Module._SetString(ptr);
   ptr2 = allocateUTF8("你好2，Emscripten！");
   Module._SetString(ptr2);
   _free(ptr2);
}
