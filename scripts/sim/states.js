const toUInt64 = function(low32, high32) {
   return (BigInt(high32) << BigInt(32)) ^ BigInt(low32)
}

const vStates = {
   props: ['update'],
   methods: {
      updateRegisters() {
         console.log("updateRegisters")
         const regBase = Module._Sim_Get_Registers();
         const NUM_REGS = 15
         for (let i = 0; i < NUM_REGS; i++) {
            const addr = regBase + i * BigInt64Array.BYTES_PER_ELEMENT
            const regVal = toUInt64(Module.HEAPU32[addr >> 2], Module.HEAPU32[(addr + 4) >> 2])
            console.log(regVal.toString(16))
         }
      },
      updateDataMemory() {
         console.log("updateDataMemory")
      },
   },
   watch: {
      update: function () {
         this.updateRegisters()
         this.updateDataMemory()
      }
   },
   template: `
      <el-divider>Y86-64 Registers</el-divider>
      <el-divider>Y86-64 Data Memory Diff</el-divider>
   `
}

export default vStates;
