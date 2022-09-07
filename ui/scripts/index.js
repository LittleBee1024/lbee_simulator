import vHome from './home/index.js'
import vSim from './sim/index.js'

const vAside = {
   props: ['routes'],
   emits: ['click'],
   methods: {
      help() {
         window.open('https://littlebee1024.github.io/learning_book/booknotes/csapp/04/','_blank')
      },
   },
   template: `
      <el-menu mode="horizontal" :ellipsis="false">
         <template v-for="(route, i) in routes">
            <el-menu-item :index="i.toString()" @click="$emit('click', route)">{{ route }}</el-menu-item>
         </template>
         <div id="help-menu" />
         <el-menu-item index="-1" @click="help">
            <el-icon><QuestionFilled /></el-icon>Help
         </el-menu-item>
      </el-menu>
   `
}

const vMain = {
   data() {
      return {
         pages: {
            'Home': Vue.shallowRef(vHome),
            'Simulator': Vue.shallowRef(vSim)
         },
         NotFoundPage: { template: '<p>Page not found</p>' }
      }
   },
   props: ['currentRoute'],
   computed: {
      CurrentComponent() {
         return this.pages[this.currentRoute] || this.NotFoundPage
      }
   },

   render() {
      return Vue.h(this.CurrentComponent)
   }
}

const vRoot = {
   data() {
      return {
         routes: ["Home", "Simulator"],
         currentRoute: "Home"
      }
   },
   components: {
      'vmain': vMain,
      'vaside': vAside,
   },
   methods: {
      go(route) {
         this.currentRoute = route
      }
   },
   template: `
      <el-container>
         <el-header>
            <vaside class="menu" :routes="routes" @click="go"/>
         </el-header>
         <el-main>
            <vmain :current-route="currentRoute" />
         </el-main>
         <el-footer> © 2022 Little Bee </el-footer>
      </el-container>
   `
}

export default vRoot;
