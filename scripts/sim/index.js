import vAsmCode from './asm_code.js'
import vBinCode from './bin_code.js'
import vLoad from './load.js'
import vRun from './run.js'

const vSim = {
   data() {
      return {
         binCode: '',
         activeAddr: -1
      }
   },
   methods: {
      setBinCode(code) {
         this.binCode = code
      },
      updateActiveAddr(addr) {
         this.activeAddr = addr
      }
   },
   components: {
      'vasmcode': vAsmCode,
      'vbincode': vBinCode,
      'vload': vLoad,
      'vrun': vRun,
   },
   template: `
      <div>
         <vasmcode @assemble="setBinCode"/>
         <vbincode :bin-code="binCode" />
         <vload :active-addr="activeAddr" />
         <vrun @update-active-addr="updateActiveAddr" />
      </div>
   `
}

export default vSim;
