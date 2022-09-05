const emptyCode = [
   {
      addr: 0x000,
      instr: '',
      comment: '',
   },
]

const vLoadCode = {
   data() {
      return {
         loadCodes: emptyCode,
         activeAddr: -1
      }
   },
   methods: {
      formatAddr(data) {
         return "0x" + data.addr.toString(16).toLocaleUpperCase().padStart(3, '0')
      },
      tableRowClassName(data) {
         if (data.row.addr === this.activeAddr) {
            return "active"
         }
         return "inactive"
      },
      load() {
         this.loadCodes = emptyCode

         Module._Sim_Reset_Recover()
         Module._Sim_Load_Code_Save()
         let codeLen = Module._Sim_Get_Code_Len()
         for (let pos = 0; pos < codeLen; pos++) {
            this.loadCodes.splice(pos, 0, {
               addr : Module._Sim_Get_Code_Addr(pos),
               instr: UTF8ToString(Module._Sim_Get_Code_Instr(pos)),
               comment: UTF8ToString(Module._Sim_Get_Code_Comment(pos)),
            })
         }
         console.log(codeLen + " bytes code was loaded")
      }
   },
   props: ['binCode'],
   template: `
      <el-button type="primary" icon="Upload" @click="load">Load</el-button>
      <el-table
         :data="loadCodes"
         style="width: 100%"
         :row-class-name="tableRowClassName">
         <el-table-column prop="addr" label="Address" width="100" :formatter="formatAddr"/>
         <el-table-column prop="instr" label="Instruction" width="200" />
         <el-table-column prop="comment" label="Comment" />
      </el-table>
   `
}

export default vLoadCode;
