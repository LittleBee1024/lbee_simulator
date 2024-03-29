const toUInt64 = function (low32, high32) {
   return (BigInt(high32) << BigInt(32)) ^ BigInt(low32)
}

const vStates = {
   props: ['update'],
   data() {
      return {
         activeCollapse: [],
         regs: [
            { 'RAX': 0x0 },
            { 'RCX': 0x0 },
            { 'RDX': 0x0 },
            { 'RBX': 0x0 },
            { 'RSP': 0x0 },
            { 'RBP': 0x0 },
            { 'RSI': 0x0 },
            { 'RDI': 0x0 },
            { 'R8' : 0x0 },
            { 'R9' : 0x0 },
            { 'R10': 0x0 },
            { 'R11': 0x0 },
            { 'R12': 0x0 },
            { 'R13': 0x0 },
            { 'R14': 0x0 },
         ],
         diffMem : [],
         cc : 4
      }
   },
   methods: {
      updateRegisters() {
         const regBase = Module._Sim_Get_Registers();
         for (let i = 0; i < this.regs.length; i++) {
            const addr = regBase + i * BigInt64Array.BYTES_PER_ELEMENT
            const regVal = toUInt64(Module.HEAPU32[addr >> 2], Module.HEAPU32[(addr + 4) >> 2])
            Object.keys(this.regs[i]).forEach(key => {this.regs[i][key] = regVal})
         }
      },
      updateDataMemory() {
         this.diffMem.length = 0

         const counts = Module._Sim_Get_Diff_Mem_Counts()
         const memAddr32Base = Module._Sim_Get_Diff_Mem_Addr()
         const memData64Base = Module._Sim_Get_Diff_Mem_Data()
         for (let i = 0; i < counts; i++) {
            const memAddr32Addr = memAddr32Base + i * Int32Array.BYTES_PER_ELEMENT
            const memData64Addr = memData64Base + i * BigInt64Array.BYTES_PER_ELEMENT
            const memAddr = Module.HEAP32[memAddr32Addr >> 2]
            const memData = toUInt64(Module.HEAPU32[memData64Addr >> 2], Module.HEAPU32[(memData64Addr + 4) >> 2])
            this.diffMem.push({[memAddr.toString(16).toLocaleUpperCase().padStart(3, '0')]: (memData.toString(16).padStart(16, '0'))})
         }
      },
   },
   watch: {
      update: function () {
         this.cc = Module._Sim_Get_CC()
         this.updateRegisters()
         this.updateDataMemory()
      }
   },
   template: `
      <el-collapse v-model="activeCollapse">
         <el-collapse-item title="Y86-64 Registers" name="1">
            <el-scrollbar>
               <el-descriptions :column="2" border>
                  <el-descriptions-item v-for="reg in regs" :label="Object.keys(reg)[0]">
                     {{ Object.values(reg)[0].toString(16).padStart(16, '0') }}
                  </el-descriptions-item>
               </el-descriptions>
               <br />
               <el-descriptions :column="6" border>
                  <el-descriptions-item label="ZF">{{(cc >> 2) & 0x1}}</el-descriptions-item>
                  <el-descriptions-item label="SF">{{(cc >> 1) & 0x1}}</el-descriptions-item>
                  <el-descriptions-item label="OF">{{cc & 0x1}}</el-descriptions-item>
               </el-descriptions>
            </el-scrollbar>
         </el-collapse-item>
         <el-collapse-item title="Y86-64 Data Memory Diff" name="2">
            <el-scrollbar>
               <el-descriptions :column="2" border>
                  <el-descriptions-item v-for="mem in diffMem" :label="Object.keys(mem)[0]">
                     {{ Object.values(mem)[0] }}
                  </el-descriptions-item>
               </el-descriptions>
            </el-scrollbar>
         </el-collapse-item>
      </el-collapse>
   `
}

export default vStates;
