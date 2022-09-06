const vStates = {
   props: ['update'],
   methods: {
      updateRegisters() {
         console.log("updateRegisters")
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
