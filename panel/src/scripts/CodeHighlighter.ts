type Plugin = {
  dependencies: number[] | undefined
  before_load?: (() => void)
  css_url: string | undefined
  js_url: string | undefined
  after_load?: (() => void)
  promise: Promise<void> | undefined
}

declare global {
  interface Window {
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    Prism: any;
  }
}

const cdn_root = "https://cdn.bootcdn.net/ajax/libs/prism/1.30.0/"

const default_plugins = new Map<number, Plugin>([
  [
    0,
    {
      js_url: cdn_root + "components/prism-core.min.js",
      css_url: cdn_root + "themes/prism-tomorrow.min.css",
      after_load: () => {
        window.Prism.manual = true
      }
    } as Plugin
  ],
  [
    1,
    {
      dependencies: [0],
      js_url: cdn_root + "plugins/toolbar/prism-toolbar.min.js",
      css_url: cdn_root + "plugins/toolbar/prism-toolbar.min.css"
    } as Plugin
  ],
  [
    2,
    {
      dependencies: [0, 1],
      js_url: cdn_root + "plugins/copy-to-clipboard/prism-copy-to-clipboard.min.js"
    } as Plugin
  ],
  [
    3,
    {
      dependencies: [0, 1],
      js_url: cdn_root + "plugins/show-language/prism-show-language.min.js"
    } as Plugin
  ],
  [
    4,
    {
      dependencies: [0, 1],
      js_url: cdn_root + "plugins/line-numbers/prism-line-numbers.min.js",
      css_url: cdn_root + "plugins/line-numbers/prism-line-numbers.min.css"
    } as Plugin
  ],
  [
    5,
    {
      dependencies: [0, 1],
      js_url: cdn_root + "plugins/line-highlight/prism-line-highlight.min.js",
      css_url: cdn_root + "plugins/line-highlight/prism-line-highlight.min.css"
    } as Plugin
  ],
  [
    6,
    {
      dependencies: [0, 1],
      js_url: cdn_root + "plugins/autoloader/prism-autoloader.min.js",
      after_load: () => {
        window.Prism.plugins.autoloader.languages_path = cdn_root + 'components/'
      }
    } as Plugin
  ],
  [
    7,
    {
      dependencies: [0],
      js_url: cdn_root + "plugins/inline-color/prism-inline-color.min.js",
      css_url: cdn_root + "plugins/inline-color/prism-inline-color.min.css"
    } as Plugin
  ]
])

export default class CodeHighlighter {
  private plugins_: Map<number, Plugin>
  private promises_: Promise<void>[]
  constructor(plugins: Map<number, Plugin> = default_plugins) {
    this.plugins_ = plugins
    this.promises_ = []
    const load = async (plugin: Plugin) => {
      if (plugin.dependencies) {
        for (const dep of plugin.dependencies) {
          const dep_plugin = plugins.get(dep)
          if (dep_plugin) {
            await load(dep_plugin) // 等待所有依赖加载完毕
          }
        }
      }
      // 插件尚未被加载，则加载当前插件
      if (!plugin.promise) {
        plugin.promise = new Promise<void>((resolve, reject) => {
          if (plugin.css_url) {
            plugin.before_load?.() // 加载前的操作
            const link = document.createElement("link")
            link.rel = "stylesheet"
            link.href = plugin.css_url
            document.head.appendChild(link)
          }
          if (plugin.js_url) {
            const script = document.createElement("script")
            script.async = true
            script.src = plugin.js_url
            script.onload = () => {
              resolve() // 加载成功兑现
              console.log(`Successfully loaded ${plugin.js_url}.`)
              plugin.after_load?.() // 加载完成后的操作
            }
            script.onerror = () => {
              console.log(`Failed to load ${plugin.js_url}.`)
              reject() // 加载失败拒绝
            }
            document.body.appendChild(script)
          } else {
            plugin.after_load?.() // 加载完成后的操作
            resolve() // 不需要加载 JS 脚本，直接兑现
          }
        });
      }
      // 插件正在加载
      return plugin.promise; // 返回承诺
    }
    this.plugins_.forEach(p => {
      this.promises_.push(load(p)); // 加载所有插件
    })
  }
  async highlight_all() {
    Promise.all(this.promises_)
      .then(() => {
        document.querySelectorAll("code[class*=language-]").forEach((code_block) => {
          window.Prism.highlightElement(code_block)
        });
      })
      .catch(e => {
        console.error(e)
      })
  }
  keep_compatibility_with_rmd() {
    // R Markdown 中使用 code-line-numbers="true" 来显示代码行号
    // 使用 code-line-numbers="1-2,3,4,5" 来高亮指定行
    document.querySelectorAll("pre > code").forEach(e => {
      const code_line_numbers = e.getAttribute("code-line-numbers")?.trim() ?? ""
      if (code_line_numbers === "true") {
        (e.parentNode as HTMLPreElement).classList.add("line-numbers");
      } else if (code_line_numbers?.match(/^(?:\d+(?:-\d+)?)(?:,\d+(?:-\d+)?)*$/)) { // 匹配高亮指定行的模式串
        (e.parentNode as HTMLPreElement).classList.add("line-numbers");
        (e.parentNode as HTMLPreElement).setAttribute("data-line", code_line_numbers);
      }
    });
  }
}


