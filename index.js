import vRoot from './scripts/index.js'

Vue.createApp({})
   .use(ElementPlus)
   .component('root', vRoot)
   .mount('#app')
