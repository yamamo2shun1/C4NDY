import DefaultTheme from 'vitepress/theme'
import './custom.css'

export default {extends: DefaultTheme,
                head:
                    [
                        [ "script", {async : "", src : "https://www.googletagmanager.com/gtag/js?id=G-EHFBVRHX4Q"} ],
                        [ "script", {}, `window.dataLayer = window.dataLayer || []; function gtag() { dataLayer.push(arguments); } gtag('js', new Date()); gtag('config', 'G-EHFBVRHX4');` ]
                    ]
}
