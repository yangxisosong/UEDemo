这个模块为项目的资产验证模块


没有任何模块需要依赖于这个模块，这个模块可以依赖于其他的模块


每种资产的验证 这个目录下 新建一个 XXXValidtor 的目录
继承 UEditorValidatorBase 实现自己的验证类即可


注意 : 这个 UEditorValidatorBase 会在引擎启动的时候 NewObject   不是用的 CDO 对象  
也就意味着这个类可以做一些自定义数据的修改 与 Cache 