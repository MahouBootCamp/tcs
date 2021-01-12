# 开发笔记

打草稿用。

## TODOs

- [x] 梳理Dispatcher、Scheduler、VehicleController、VehicleAdapter相关业务逻辑
  - [x] 订单逻辑
  - [x] DriveOrder执行逻辑
- [x] 命名风格修改：成员函数一律大写
- [x] 完善Service设施
  - [x] Service开发
  - [x] 代替系统中所有直接对地图、订单的写操作
- [x] 完善事件链
  - [x] Dispatcher触发机制
- [ ] 搭建一个可用测试的Kernal
  - [ ] 添加一个周期性的Dispatch任务
  - [x] 各组件的构造函数整理
  - [ ] Kernal功能完善
- [ ] 对各组件的单元测试
- [ ] 多线程约束
- [ ] 系统测试
- [ ] 添加const约束
- [ ] 进一步开发：
  - [ ] 低电量载具充电、空闲载具停泊逻辑。
  - [ ] 载具问题处理

## 订单执行逻辑梳理

`Dispatcher`被设置为每间隔一段时间便运行一次（OpenTCS中默认设置为10s一次）。

### 正常普通订单流程

#### Step1 创建

一个正常工作的普通订单由用户自行创建。在创建后该订单的`TransportOrderState`被设置为`kRaw`状态，并等待下一个`DispatchTask`的执行。（**TODO**：在更高层级处创建订单后应当主动调用Dispatcher的`Dispatch()`函数。）

#### Step2 可寻路性检测

在`Phase0CheckNewOrder`中，系统调用Router检测所有`kRaw`状态的订单的可寻路性，如果不可则设置为`kUnroutable`，订单生命结束；可则设置为`kDispatchable`，等待后续处理。

#### Step3 分配

在`Phase4AssignFreeOrder`中，系统检测所有设置为`kDispatchable`的订单，并为之分配载具。

在这一步中，如果可用载具比订单少，则根据载具当前位置计算订单执行花费，优先执行低花费订单；否则根据订单计算每台载具执行该订单的花费，优先选择花费少的载具。

如果当前无法执行，则跳过该订单。该订单将等待至下一次Phase4尝试分配。

如果可以执行，则会将订单、载具配对，并尝试分配，进入Step4。

#### Step4 分配第一步

可用载具本身可能处于两个状态：空闲或正在执行可撤回订单。可撤回订单指的是系统分配的停泊订单，这些订单用于让空闲载具停泊至系统指定的停泊点，以避免拥挤在任务执行点位。

如果载具正在执行可撤回订单，则在这一步中会撤回该系统订单。随后载具与订单会被加入ReserveOrderPool中等待执行。流程跳转至Step6。

如果载具空闲，则为之分配订单，包括：

- 从ReserveOrderPool中移除该订单（如果有的话）。
- 更新载具状态为`kProcessingOrder`。
- 更新订单状态为`kBeingProcessed`。
- 更新载具正在执行的订单ID。
- 更新订单的载具ID。
- 更新订单的DriveOrders为Router找到的最优解，并将进度标记设置为0。
- 在Router缓存载具与对应的DriveOrders。
- 获取第一个DriveOrder并检查可否跳过。
  - 可，则设置载具状态为`kAwaitingOrder`。
  - 否，则通过VehicleController设定要执行的DriveOrder。

随后载具等待下一步或开始执行当前DriveOrder直至完毕，这一部分将由VehicleController和Scheduler共同控制。之后进入Step5。

#### Step5 分配下一步/结束订单

在`Phase2AssignNextDriveOrder`中，Dispatcher为载具分配下一步动作。

载具执行完毕或跳过了一个DriveOrder后，即会被设定为`kAwaitingOrder`。检测所有当前为该状态的载具，并更新这些载具所执行订单的进度标记加1。

检测进度标记，如果该标记与路径数一致，则说明所有DriveOrders均执行完毕，即订单执行完毕。这时：

- 更新订单状态为`kFinished`。
- 更新载具状态为`kIdle`。
- 更新载具执行订单为空。
- 清除Router缓存。

否则说明有下一DriveOrder。检查该DriveOrder是否可以跳过，可则直接重复Phase2。否则将该DriveOrder推送给VehicleController并更新载具状态为`kProcessingOrder`。

#### Step6 预留订单

在`Phase3AssignReservedOrder`中，Dispatcher为预留订单进行分配。

对于空闲载具，系统检测是否有对应的预留订单。如果有则重新为其计算路径，并进入Step4。

### 撤销订单流程

如果订单尚未被分配至载具，则直接将其状态更新为`kFailed`。

如果订单正在处理，则将其状态设置为`kWithdraw`。并调用VehicleController的`AbortDriveOrder`。这会停止控制器为载具进一步请求资源的行为，并取消除载具当前步骤（含一条路径和一个目标点）的所有资源请求。

在下一次`Phase1FinishWithdrawal`中，对于状态为`kAwaitingOrder`的载具（即其完成了被取消的DriveOrder的残余指令），如果订单状态为`kWithdraw`，则撤销订单：

- 订单状态更新为`kFailed`
- 载具运行状态更新为`kIdle`
- 载具订单更新为空
- 清空Router缓存

### 系统内部订单：充电/停车订单

<!--UNDONE 这一部分逻辑需要进一步考虑-->

## 资源管理/载具控制逻辑梳理

VehicleController提供了控制载具的一系列接口，其依赖于一个用户实现的VehicleAdapter（预设了若干接口）来实现功能。它将DriveOrder分解为一个又一个MovementCommand并推送给VehicleAdapter。VehicleAdapter负责与载具进行实际通讯，并将系统指令转化为载具指令（故需要载具开发人员自行实现）。

Scheduler集中进行系统资源管理。在执行一个MovementCommand前，VehicleController会首先向Scheduler请求资源并等待Scheduler调用它的`AllocationSuccessful()`接口（意味着资源已经被分配给它），随后它再将MovementCommand推送给VehicleAdapter。这时候载具便可以放心执行该指令而无需担心与其他载具的冲突。

### 载具初始化

VehicleController的构造函数会调用VehicleAdapter的`Enable()`函数以连接载具。在连接成功后，还需为载具配置初始位置。调用`SetInitPosition()`函数会向Scheduler请求初始位置的资源，并将初始位置推入请求资源栈。在这之后载具才可以将级别调整至`kUtilized`。

### 执行DriveOrder逻辑

#### Step1

外部调用`SetDriveOrder()`为控制器指定要执行的DriveOrder。控制器会声明其将要申请的资源，设置载具状态，将执行步骤（这里指Step）设置为0，申请第一个Step的资源。

#### Step2

Scheduler通过了该资源申请后，会调用控制器的`AllocationSuccessful()`。这时控制器会比对刚分配的资源与自己请求资源是否一致。若不一致，返回失败。

若一致，则检查是否有待执行的指令。如有则将刚才的资源推入请求资源栈，并将该指令推送给Adapter。随后如果Adapter可以接收更多指令缓存，则申请下一个Step的资源。

> 请求资源与载具的行动未必保持一致。

#### Step3

Adapter在指令栈不为空时取出一个指令并执行。顺利执行的过程中，Adapter可能会激活`UpdatePositionEvent`、`FinishCommandEvent`事件。

Controller处理`UpdatePositionEvent`时，设置载具的新位置。

Controller处理`FinishCommandEvent`时，会将请求资源栈的首元素弹出并通知Scheduler释放。

如果还有后续指令，则继续尝试`AllocateForNextCommand`。

如果没有后续指令，则意味着整个DriveOrder结束，重置载具进度为0，载具执行状态为`kAwaitingOrder`。

### 撤销DriveOrder逻辑

### DriveOrder执行失败逻辑

<!--UNDONE-->

## 系统逻辑梳理

### 依赖关系与初始化

这部分梳理内部组件之间复杂的逻辑关系。

1. 最基础的组件显然是由MapBuilder构造的Map。这部分被独立在Kernal之外构造，并作为构造函数参数传入Kernal。
2. 随后是OrderPool，仅依赖于Map。
3. Executor是一个独立组件，不依赖于任何东西。
4. 基于这两个基础组件，有三个Service用于控制对这两个基础组件的读写。多线程控制尚未加入。
5. Router依赖于MapService。
   1. Router的内部组件ShortestPathAlgorithm需要Map进行初始化。
6. Scheduler依赖于MapService与Executor。
   1. Scheduler有一个内部组件ReservationPool，无依赖。
7. ControllerPool依赖于Map、VehicleService、Scheduler。（因为DefaultController依赖于这些组件，尽管DefaultController要到用户调用Kernal方法时才创建。）
8. Dispatcher直接依赖于除Scheduler以外的所有组件（也间接依赖于Scheduler）。
   1. ReserveOrderPool为Dispatcher一独立内部组件。
   2. UniversalDispatchUtil与Dispatcher依赖相同。
   3. 各Phase的依赖均为Dispatcher依赖的子集

各组件的初始化也应当按照上述的顺序进行。

### `KernalState::kIdle`

### `KernalState::kOperating`

### `KernalState::kExit`
