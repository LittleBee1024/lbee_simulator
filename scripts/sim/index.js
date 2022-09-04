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
         <vasmcode :bin-code="binCode" @assemble="setBinCode"/>
         <vbincode :bin-code="binCode" />
      </div>
   `
}

export default vSim;
