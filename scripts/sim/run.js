const vRun = {
   data() {
      return {
         speed: 50,
         isRunning: false,
         runIntervalID: 0,
         activeAddr: -1,
         state: "OK",
      }
   },
   emits: ['updateActiveAddr'],
   methods: {
      update() {
         this.$emit("updateActiveAddr", this.activeAddr)
      },
      run() {
         if (!this.isRunning)
         {
            this.isRunning = true
            this.runIntervalID = setInterval(this.step, this.speed * 10);
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
   template: `
      <el-row>
         <el-button type="primary" icon="VideoPlay" @click="run">Run</el-button>
         <el-button type="primary" icon="ArrowRight" @click="step">Step</el-button>
         <el-button type="warning" icon="VideoPause" @click="stop">Stop</el-button>
         <el-button type="warning" icon="RefreshLeft" @click="reset">Reset</el-button>
      </el-row>
      <div class="slider-speed">
         <span class="demonstration">Run Speed</span>
         <el-slider v-model="speed" />
      </div>
      <el-divider>Y86-64 Registers</el-divider>
      <el-divider>Y86-64 Data Memory Diff</el-divider>
   `
}

export default vRun;


