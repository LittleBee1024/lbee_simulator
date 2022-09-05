import vAsmCode from './asm_code.js'
import vBinCode from './bin_code.js'
import vLoadCode from './load_code.js'

const vSim = {
   data() {
      return {
         binCode: '',
         activeAddr: 0
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
      'vloadcode': vLoadCode,
   },
   template: `
      <div>
         <vasmcode @assemble="setBinCode"/>
         <vbincode :bin-code="binCode" />
         <vloadcode :active-addr="activeAddr" />
      </div>
   `
}

export default vSim;
