import vAsmCode from './asm_code.js'
import vBinCode from './bin_code.js'

const vSim = {
   data() {
      return {
         binCode: ''
      }
   },
   methods: {
      setBinCode(code) {
         this.binCode = code
      }
   },
   components: {
      'vasmcode': vAsmCode,
      'vbincode': vBinCode,
   },
   template: `
      <div>
         <el-row :gutter="20">
            <el-col :span="10">
               <vasmcode :bin-code="binCode" @assemble="setBinCode"/>
            </el-col>
            <el-col :span="14">
               <vbincode :bin-code="binCode" />
            </el-col>
         </el-row>
      </div>
   `
}

export default vSim;
