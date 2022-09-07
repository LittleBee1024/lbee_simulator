import vStates from "./states.js"

const vRun = {
   data() {
      return {
         speed: 50,
         isRunning: false,
         runIntervalID: 0,
         activeAddr: -1,
         state: "OK",
         updateStates: 0,
      }
   },
   emits: ['updateActiveAddr'],
   methods: {
      update() {
         this.$emit("updateActiveAddr", this.activeAddr)
         this.updateStates = !this.updateStates
      },
      run() {
         if (!this.isRunning)
         {
            this.isRunning = true
            this.runIntervalID = setInterval(this.step, 1000 - this.speed * 10);
         }
      },
      step() {
         this.activeAddr = Module._Sim_Get_Cur_PC()
         this.state = UTF8ToString(Module._Sim_Step_Run())
         this.update()
      },
      stop() {
         if (this.isRunning)
         {
            this.isRunning = false
            clearInterval(this.runIntervalID);
         }
      },
      reset() {
         this.stop()
         Module._Sim_Reset_Recover()
         this.activeAddr = -1
         this.state = "OK"
         this.update()
      }
   },
   computed: {
      statusType: function() {
         if (this.state == "OK")
            return ''
         if (this.state == "HALT")
            return 'warning'
         return 'danger'
      }
   },
   components: {
      'vstates': vStates,
   },
   template: `
      <el-row id="run">
         <el-button type="primary" :disabled="isRunning" icon="VideoPlay" @click="run">Run</el-button>
         <el-slider v-model="speed" />
         <span class="demonstration">Speed</span>
      </el-row>
      <br />
      <el-row>
         <el-button type="primary" :disabled="isRunning" icon="ArrowRight" @click="step">Step</el-button>
         <el-button type="warning" :disabled="!isRunning" icon="VideoPause" @click="stop">Stop</el-button>
         <el-button type="warning" :disabled="false" icon="RefreshLeft" @click="reset">Reset</el-button>
      </el-row>
      <br />
      <el-row>
         <el-tag :type="statusType" size="large" effect="light">Simulator Status: {{state}}</el-tag>
      </el-row>
      <br />
      <vstates :update="updateStates"/>
   `
}

export default vRun;


