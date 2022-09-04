import vHome from './home/index.js'
import vSim from './sim/index.js'

const vAside = {
   props: ['routes'],
   emits: ['click'],
   template: `
      <el-menu mode="horizontal">
         <template v-for="(route, i) in routes">
            <el-menu-item :index="i.toString()" @click="$emit('click', route)">{{ route }}</el-menu-item>
         </template>
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
