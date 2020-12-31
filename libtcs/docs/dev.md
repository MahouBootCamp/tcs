# 开发笔记

打草稿用。

## 逻辑

TransportOrderState：

- kRaw：刚创建
- kActive：通过Dispatcher检查，等待依赖订单完成
- kDispatchable：无依赖订单，随时可以分配
- kBeingProcessed：正在执行
- kWithdraw：被撤回（人为或系统自动）
- kFinished：完成
- kFailed：失败
- kUnRoutable：无法寻路

VehicleState：

- kUnknown：未知
- kUnavailable：不可用
- kError：错误
- kIdle：待机
- kExecuting：运行中
- kCharging：充电中

ProcessState：

- kIdle：待机
- kAwaitingOrder：等待指令
- kProcessingOrder：执行指令

整个控制系统的工作都基于以上这几个状态构成的状态机。由于OpenTCS将一个完整的用户创建的订单称为TransportOrder，又将订单的步骤（到某点做某事为一个步骤）称为DriveOrder，而这两者又均简称为Order，因此在理解上造成了混乱。

`VehicleState`用于指示载具当前状态。这是一个由载具自行掌控的状态（Adapter负责）。系统的其他部分只读取以判断该载具是否可用。

`ProcessState`用于指示载具执行DriveOrder的状态。Dispatcher在将一个DriveOrder推送给载具的Controller后，便将该状态设为kProcessingOrder。Controller会在DriveOrder执行结束后将该状态设为kAwaitingOrder。随后Dispatcher处若判断整个订单执行完毕，会将其设置为kIdle。
