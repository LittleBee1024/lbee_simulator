const vRun = {
   data() {
      return {
         activeAddr: -1,
         speed: 50
      }
   },
   emits: ['updateActiveAddr'],
   methods: {
      run() {
         console.log("run")
      },
      step() {
         console.log("step")
         this.activeAddr++
         this.$emit("updateActiveAddr", this.activeAddr)
      },
      stop() {
         console.log("stop")
      },
      reset() {
         console.log("reset")
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


