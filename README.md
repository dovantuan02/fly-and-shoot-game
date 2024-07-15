# FLY - AND - SHOOT GAME trên AK - Embedded Base Kit

![BANNER GAME](https://github.com/DoVanTuan2805/_fly-and-shoot-game/blob/main/resource/images/banner_game.png)

**I. Giới thiệu**

**1.1. Giới thiệu sơ lược về phần cứng.**

![HARDWARE LAYOUT](https://github.com/DoVanTuan2805/_fly-and-shoot-game/blob/main/resource/images/hardware_layout.png)

- AK-Embedded Base Kit là  evaluation kit dành cho các bạn học phần mềm nhúng nâng cao và muốn thực hành với Event - Driven.
- Các ngoại vi và MCU được tính hợp vào trong Kit:
    - Vi điều khiển chính STM32 ARM-based 32-bit MCU: [STM32L151C8T6](https://www.st.com/en/microcontrollers-microprocessors/stm32l151c8.html)
    - Truyền nhận không dây 2.4Ghz RF Transceiver ICs: [NRF24L01P-R](https://www.nordicsemi.com/products/nrf24-series)
    - Giao tiếp có dây trong công nghiệp RS485 50Mbps: [THVD1450DR](https://www.ti.com/product/THVD1450/part-details/THVD1450DR)
    - Tích hợp loa dùng để làm game Buzzers Magnetic: [MLT-8530](https://www.lcsc.com/product-detail/Buzzers_Jiangsu-Huaneng-Elec-MLT-8530_C94599.html)
    - Tích hợp NOR FLASH (32MB): [W25Q256JVEIQTR](https://www.winbond.com/hq/product/code-storage-flash-memory/serial-nor-flash/index.html?__locale=en&partNo=W25Q256JV)
    - Kết nối với các mạch ứng dụng chuẩn [Seeedstudio Grove Header HY-4AW](https://wiki.seeedstudio.com/Grove_System/)
    - Tích hợp console qua USB type C sử dụng chip USB UART Converter: [CH340E](http://www.wch-ic.com/products/CH340.html)
    - Nguyên lí của phần cứng: AK-Embedded Base Kit.
- Trò chơi "Fly and Shoot" là một tựa game kinh điển đã xuất hiện từ lâu trong làng game. Đặc biệt, hiện nay trò chơi này đã được tái tạo trên nền tảng AK-Embedded Base Kit, với việc tích hợp một hệ thống quản lý nhiệm vụ, tín hiệu, bộ đếm thời gian, và tin nhắn kết hợp với một cơ chế Event-Driven.
- Bằng cách sử dụng cơ chế Event-Driven, trò chơi có thể phản ứng linh hoạt theo sự kiện diễn ra trong trò chơi một cách tự nhiên, tạo ra trải nghiệm chơi game mượt mà và hấp dẫn hơn cho người chơi. Điều này giúp tăng cường sự tương tác và động lực cho người chơi trong quá trình tham gia vào trò chơi "Fly and Shoot".

**1.2. Màn hình Menu**

![SCREEN MENU](https://github.com/DoVanTuan2805/_fly-and-shoot-game/blob/main/resource/images/screen_menu.png)

- PLAY: Bắt đầu trò chơi.
- SETTING: Cài đặt cái thông số cho trò chơi mode(chế độ), missile(đạn), âm thanh(bật/tắt).
- TUTORIAL: QR hướng dẫn trò chơi.
- HISTORY: Lưu lịch trò điểm sau những lần chơi.
- EXIT: Thoát màn hình menu.

**1.3. Các đối tương hoạt động trong game.**

![INTRO OBJECT IN GAME](https://github.com/DoVanTuan2805/_fly-and-shoot-game/blob/main/resource/images/game_object.png)

| Đối tượng | Ý nghĩa | Người chơi tác động | Tự động tác động |
| --- | --- | --- | --- |
| PLANE | Tàu bay | Di chuyển tàu bay đi lên  | Di chuyển tàu bay đi xuống, chạm quặng, bom sẽ tạo ra vụ nổ và kết thúc game |
| MISSILE | Đạn | Bắn ra đạn phá hủy quặng, bom | Di chuyển, tạo ra vụ nổ khi chạm quặng, bom, tạo ra điểm khi chạm quặng |
| MINE | Quặng | Không | Di chuyển, tự động tạo ra |
| BOM | Bom | Không | Di chuyển, tự động tạo ra |
| EXPLOSION | Vụ Nổ | Không | Vụ nổ xảy ra khi đạn chạm quặng hoặc bom |
| TUNNEL WALL | Đường hầm | Không | Di chuyển bản đồ |

**1.4. Cách chơi của game.**

Người chơi có thể điều khiển tàu đi lên (UP) hoặc để tàu bay tự động rơi xuống để né các quặng và bom không chạm vào máy bay. Đồng thời cũng không được để tàu bày chạm vào đường hầm (TUNNEL WALL). 

Tăng độ khó bằng cách vào cài đặt và chỉnh mode (EASY, NORMAL, HARD).

Có thể bắn đạn bằng cách nhấn nút MODE khi đạn chạm trúng các quặng sẽ tạo ra điểm. Số đạn có thể tùy chỉnh được trong SETTING và được giới hạn tối đa là 5 viên.

*Cách tính điểm:*

- Quặng nhỏ sẽ có 1 điểm khi bắn trúng.
- Quặng lớn sẽ có 2 điểm khi bắn trúng.
- Bom sẽ không có điểm khi bắn trúng.

Số điểm sẽ được lưu khi trò chơi kết thúc. Khi trò chơi kết thúc nhấn MODE để chơi tiếp, nhấn DOWN để quay trở về menu.

**II. Thiết kế các Task, Signal trong game.**

**2.1. Event - Driven hoạt động như thế nào ?**

![INTRO EVENT DRIVEN](https://github.com/DoVanTuan2805/_fly-and-shoot-game/blob/main/resource/images/intro_event_driven.png)

                                 Nguồn: Automatic Control Programming

- Event Driven nói một cách dễ hiểu là một hệ thống gửi thư (gửi message) để thực thi các công việc. Trong đó, Task đóng vai trò là người nhận thư, Signal đại diện cho nội dung công việc. Task & Signal nền tảng của một hệ Event Driven.
- Thông thường mỗi Task sẽ nhận một nhóm công công việc nào nào đó, ví dụ: quản lý state-machine, quản lý hiển thị của màn hình, quản lý việc cập nhật phần mềm, quản lý hệ thống watchdog ...
- Message được chia làm 2 loại chính, Message chỉ chứa Signal, hoặc vừa chứa Signal và Data. Message tương đương với Signal.
- Chỗ thực thi một công việc nào đó thì mình gọi là Handler.

Chi tiết các khái niệm các bạn tham khảo tại bài viết: AK Embedded Base Kit - STM32L151 - Event Driven: Task & Signal

**2.2. Task và Signal trong Fly - And - Shoot.**

*Các Task và Priority trong game:*

| TASK ID | PRIORITY | HANDLER |
| --- | --- | --- |
| FS_GAME_TASK_DISPLAY_GAME_ON_ID | TASK_PRI_LEVEL_4	 | task_scr_fs_game_on_handle |
| FS_GAME_TASK_PLANE_ID				 | TASK_PRI_LEVEL_4	 | task_fs_plane_hanle		 |
| FS_GAME_TASK_MISSLE_ID				 | TASK_PRI_LEVEL_4	 | task_fs_missile_handle		 |
| FS_GAME_TASK_WALL_ID				 | TASK_PRI_LEVEL_4 | task_fs_wall_handle		 |
| FS_GAME_TASK_EXPLOSION_ID			 | TASK_PRI_LEVEL_4	 | task_fs_explosion_handle	 |
| FS_GAME_TASK_OBSTACLE_ID				 | TASK_PRI_LEVEL_4 | task_fs_obstacle_handle			 |
| FS_GAME_TASK_DISPLAY_GAME_OVER_ID	 | TASK_PRI_LEVEL_4	 | task_scr_fs_game_over_handle	 |


![Intro Object Game](https://github.com/DoVanTuan2805/_fly-and-shoot-game/blob/main/resource/images/intro_object_game.png)

        			    Chi tiết các signal trong các Task:

*Task ID*:  Mỗi task được tạo ra cho một đối tượng khác nhau trong hệ thống và có nhiệm vụ nhận các công việc khác nhau. Các task hoàn toàn tác biệt luồng logic với nhau.

*Priority*: Mức độ ưu tiên của các task. Như trong ảnh bên trên ta có thể thấy độ ưu tiên của các task là bằng nhau. Tức là khi các task có priority bằng nhau đều có sự kiện thì hệ thống sẽ sử lí task nào vào trước.

*Handler*: Là nơi sử lí các tín hiệu của sự kiện khi xảy ra tác động.

Signal: Mỗi task sẽ có nhiều signal khác nhau nhằm mục đích xử lí các nhiệm vụ khác nhau của các đối tượng.                              

Ví dụ: Ở task missile sẽ có signal (FS_GAME_MISSILE_PUSH_SIG) sẽ có nhiệm vụ tạo ra đạn, còn (FS_GAME_MISSLE_MOVE_SIG) sẽ có nhiệm vụ di chuyển nếu có đạn.

Các task có thể đồng bộ hóa và trao đổi dữ liệu bằng cách bắn message qua các task đi kèm với signal.

Có 2 loại message khác nhau:

- Message chỉ mang theo Signal, không chứa data.
- Message chứa Signal và mang theo cả data.

**2.3. Sơ đồ quá trình của game**

![Sequence After Game On](https://github.com/DoVanTuan2805/_fly-and-shoot-game/blob/main/resource/images/Sequence/sequence_after_game_on.png)

                                        Hình 2.3.1: Sequence after game on

![Sequence Game Active](https://github.com/DoVanTuan2805/_fly-and-shoot-game/blob/main/resource/images/Sequence/sequence_game_active.png)

                                        Hình 2.3.2: Sequence when game run

![Sequence Game Over](https://github.com/DoVanTuan2805/_fly-and-shoot-game/blob/main/resource/images/Sequence/sequence_game_over.png)

                                        Hình 2.3.3: Sequence game over

*Action after run game*

| SCREEN_ENTRY | Khi người dùng bắt đầu chơi game |
| --- | --- |
| FS_GAME_DISPLAY_ON_TICK | Signal do timer gửi đến với chu kì 100ms cập nhật lại màn hình |
| FS_GAME_PLANE_UP_SIG | Cài đặt thông số mặc định cho tàu bay |
| FS_GAME_PLANE_ON_TICK_SIG | Signal do timer gửi đến với chu kì 100ms giúp tàu bay đi xuống |
| FS_GAME_OBSTACLE_RESET_SIG | Cài đặt thông số mặc định cho các vật cản |
| FS_GAME_OBSTACLE_ON_TICK_SIG | Signal do timer gửi đến với chu kì 150ms dùng để di chuyển các quặng, bom |
| FS_GAME_OBSTACLE_PUSH_SIG | Signal do timer gửi đến với chu kì 800ms dùng để tạo thêm quặng hoặc bom |
| FS_GAME_MISSLE_RESET_SIG | Cài đặt lại tất cả các thông số cho đạn |
| FS_GAME_WALL_RESET_SIG | Cài đặt lại tất cả các thông số cho hầm |
| FS_GAME_WALL_ON_TICK_SIG | Signal do timer gửi đến với chu kì 100ms dùng để di chuyển hầm |
| FS_GAME_EXPLOSION_RESET_SIG | Cài đặt lại tất cả các thông số cho vụ nổ |
| FS_GAME_EXPLOSION_ON_TICK_SIG | Signal do timer gửi đến với chu kì 150ms dùng để tạo hoạt ảnh cho vụ nổ |
| fs_state_game | Biến lưu trạng thái của game |

*Action normal*

| FS_GAME_DISPLAY_ON_TICK | Signal do timer gửi đến với chu kì 100ms |
| --- | --- |
| FS_GAME_PLANE_CRASH_SIG | Signal kiểm tra tàu bay có chạm vào bom, quặng hay tường  |
| FS_GAME_DISPLAY_OVER_ON_TICK | Signal tạo ra khoảng chờ để chuyển đến màn hình game over |
| FS_GAME_MISSLE_ON_TICK_SIG | Signal do timer gửi đến với chu kì 100ms dùng để di chuyển bom |
| FS_GAME_MISSLE_RESET_SIG | Signal kiểm tra đạn có chạm vào bom hay quặng  |
| RELOAD SCREEN | Giúp cập nhật lại toàn bộ màn hình |

*Action player*

| FS_GAME_PLANE_UP_SIG | Player nhấn [Up] tạo ra signal giúp tàu bay đi lên |
| --- | --- |
| FS_GAME_MISSLE_PUSH_SIG | Player nhấn [Mode] tạo ra signal giúp bắn ra đạn |

*Action game over*                          

| timer_remove | Xóa các timer chạy cho các đối tượng trong game |
| --- | --- |
| Write score to eeprom and read history. | Lưu lại điểm đạt được trong game vào trong bộ nhớ eeprom |

Như ở hình trên:

Game sẽ được chia làm 3 phần chính:

**Phần 1:** Quá trình cài đặt thông số cho các đối tượng trong game, và timer cho cho đối tượng trong game

**Phần 2:** Quá trình bắt đầu game chạy.

- Khi không có tác động của người dùng
- Khi có tác động của người dùng

**Phần 3:** Quá trình game kết thúc. 

*Vì vậy các đối tượng của game cũng chia làm 3 phần chính.*

**III. Chi tiết code các đối tượng trong game**

 **3.1 Tàu bay (Plane)**

Mỗi đối tượng sẽ có những thông số để điều khiển những đối tượng đó.

```cpp
typedef struct
{
    int8_t x;
    int8_t y;
}fs_game_coordinate_t;

typedef struct {
    bool visible;
    fs_game_coordinate_t coordinate;
} fs_plane_info_t;

fs_plane_infor_t fs_plane;
```

Ở đoạn code trên thông số của tàu bay sẽ được lược vào kiểu dữ liệu struct mục đích nhằm để dễ quản lí. Ở trong struct trên có lồng thêm struct “fs_game_coordinate_t” nhằm để quản lí dễ dàng về tạo độ cho tàu bay. Biến “visible“ là nhằm quản lí trạng thái ẩn hiện của tàu bay.  

![Sequence Plane](https://github.com/DoVanTuan2805/_fly-and-shoot-game/blob/main/resource/images/Sequence/sequence_plane.png)

                                         Hình 3.1.1: Plane sequence

Giải thích plane sequence:

*Như ở cuối mục 2.3 ta được biết sẽ có phần chính. Thì ở phần đối tượng tàu bay cũng sẽ được chia ra làm 3 phần:*

**Phần 1:** Trước khi vào game sẽ có signal SCREEN_ENTRY khi đó sẽ cài đặt timer 100ms sẽ bắn message đến FS_GAME_PLANE_ON_TICK_SIG và FS_GAME_PLANE_CRASH_SIG.

**Phần 2:** 

*Khi không có tác động của người chơi:*

Tàu bay mỗi 100ms sẽ di chuyển xuống và đồng thời cũng kiểm tra tàu bay có va chạm không?

- Có: Sẽ bắn message đến FS_GAME_EXPLOSION_PUSH_SIG kèm theo tạo độ vụ nổ, đồng thời cũng cài đặt thời gian delay 2000ms để bắn message đến FS_GAME_DISPLAY_ON_TICK nhằm tạo ra hiệu ứng thời gian chờ trước khi game over.
- Không: Thì sẽ không làm gì.

*Khi có tác động của người chơi:*

Khi người chơi nhấn nút [UP] sẽ bắn message đến FS_GAME_PLANE_UP_SIG giúp cho tàu bay đi lên.

 

**Phần 3:** Khi có tín hiệu FS_GAME_DISPLAY_OVER_ON_TICK sẽ cài đặt lại thông số cho tàu bay.

Đoạn code trong source:

Tín hiệu FS_GAME_PLANE_SETUP_SIG:

```cpp
static inline void fs_game_plane_setup() {
    fs_plane.coordinate.x = 5;
    fs_plane.coordinate.y = 15;
    fs_plane.visible = true;
}
```

Tín hiệu FS_GAME_PLANE_DOWN_SIG :

```cpp
static inline void fs_game_plane_down() {
    fs_plane.coordinate.y += FS_PLANE_Y_DOWN;
    if (fs_plane.coordinate.y > MAP_HEIGHT) {
        fs_plane.visible = false;
        fs_plane.coordinate.y = MAX_LCD_HEIGHT;
    }
}
```

Tín hiệu FS_GAME_PLANE_UP_SIG :

```cpp
static inline void fs_game_plane_up() {
    fs_plane.coordinate.y -= FS_PLANE_Y_UP;
}
```

Tín hiệu FS_GAME_PLANE_CRASH_SIG :

```cpp
static inline void fs_game_wall_crash() {
	// code
}
static inline void fs_game_obstacle_crash() {
	// code
}
static inline void fs_game_plane_crash() {
    if(fs_state_game == FS_GAME_ON){
        fs_game_obstacle_crash();
        fs_game_wall_crash();
    }
}
```

Cài timer khi tàu bay va chạm :

```cpp
timer_set(FS_GAME_TASK_DISPLAY_GAME_OVER_ID, FS_GAME_DISPLAY_OVER_ON_TICK, AC_GAME_OVER_INTERNAL, TIMER_ONE_SHOT);
```

Lấy tọa độ khi tàu bay va chạm :

```cpp
fs_explosion.coordinate.x = fs_plane.coordinate.x;
fs_explosion.coordinate.y = fs_plane.coordinate.y;
fs_explosion.ver = VER_I;
task_post_pure_msg(FS_GAME_TASK_EXPLOSION_ID, FS_GAME_EXPLOSION_PUSH_SIG);
```

**3.2. Đạn (Missile)**

Tương tự như tàu bay, đạn cũng có những biến quản lí các thông số:

```cpp
class fs_missile_info_t {
   private:
   public:
    bool visible;
    fs_game_coordinate_t coordinate;
    fs_missile_info_t(uint8_t x, uint8_t y, bool visible) {
        this->coordinate.x = x;
        this->coordinate.y = y;
        this->visible = visible;
    }
};

vector<fs_missile_info_t> fs_vec_missile;
```

Chức năng biến của missile cũng tương tự như “Plane”. Xem lại hình 3.1.1 để hiểu rõ hơn.

![Sequence Missile](https://github.com/DoVanTuan2805/_fly-and-shoot-game/blob/main/resource/images/Sequence/sequence_missile.png)

                                      Hình 3.2.1 Missile sequence

Giải thích missile sequence:

**Phần 1:** Trước khi bắt đầu vào game sẽ có signal “SCREEN_ENTRY” và sẽ đặt lại các thông số cho missile, và xóa missile trước đó nếu có. Và sẽ cài đặt timer cho mỗi 100ms sẽ bắn message đến FS_GAME_MISSILE_ON_TICK, FS_GAME_MISSILE_CRASH_SIG. 

**Phần 2:** 

*Khi không có tác động của người chơi:*

- Đạn (nếu có) sẽ di chuyển từ trái sang phải mỗi 100ms.
- Kiểm tra đạn (nếu có) có va chạm với bom, quặng hay không?
    - Có: Sẽ bắn message đến FS_GAME_EXPLOSION_PUSH_SIG kèm theo tạo độ vụ nổ và cộng điểm cho người chơi.
        - Quặng 1: sẽ được cộng 1 điểm.
        - Quặng 2: sẽ được cộng 2 điểm.
        - Bom: sẽ không có điểm.
    - Không: Thì sẽ không làm gì.

*Khi có tác động của người chơi:*

- Khi người chơi nhấn nút [MODE] sẽ bắn message đến FS_GAME_MISSILE_PUSH_SIG giúp tạo ra đạn với số lượng tùy vào trong cài đặt. Tối đa sẽ 5 viên

 **Phần 3:** Khi có tín hiệu FS_GAME_DISPLAY_OVER_ON_TICK sẽ xóa hết đạn (nếu có) và cài đặt lại thông số cho đạn.

Đoạn code trong source:

Tín hiệu FS_GAME_MISSILE_RESET_SIG:

```cpp
// clear all missile available
static inline void fs_game_missle_reset() {
		//code
}
```

Tín hiệu FS_GAME_MISSILE_PUSH_SIG:

```cpp
// add missile to missile managerment
static inline void fs_game_missle_push() {
		//code
}
```

Tín hiệu FS_GAME_MISSILE_ON_TICK_SIG:

```cpp
// move all missile to right screen
static inline void fs_game_missle_move() {
		// code
}
```

Tín hiệu FS_GAME_MISSILE_CRASH_SIG:

```cpp
// missile hits mine or bom
static inline void fs_game_missle_crash() {
		// code
}
```

*****  Do đoạn code khá dài ae cần có thể vào source xem nhé..*

Code bắn message đến FS_GAME_EXPLOSION_PUSH_SIG kèm tạo độ để tạo vụ nổ :

```cpp
fs_explosion.coordinate.x = fs_vec_obstacle[k].coordinate.x;     
fs_explosion.coordinate.y = fs_vec_obstacle[k].coordinate.y;
fs_explosion.ver = FS_EXPLOSION_VER_I;

task_post_pure_msg(FS_GAME_TASK_EXPLOSION_ID, FS_GAME_EXPLOSION_PUSH_SIG);
```

**3.3 Vật cản (Obstacle)**

Các Ae thắc mắc các quặng hay bom từ ở đâu ra thì có đọc đoạn này nhé.

Biến quản lí các obstacle:

```cpp
/*
*	TABLE MANAGER OBSTACLE (ID, COORDINATE, BITMAP, SCORE)
*	obstacle_tbl:
*		Size Bitmap MUST 5x5 pixel
*/
#define FS_OBSTACLE_TBL		(3)
const fs_obstacle_info_t obstacle_tbl[FS_OBSTACLE_TBL] = {
	{FS_BOM_ID		,	{0,0},	bom_icon	,	0},
	{FS_MINE_I_ID	,	{0,0},	mine_I_icon	,	1},
	{FS_MINE_II_ID	,	{0,0},	mine_II_icon,	2},
};
/*
*   fs_vec_obstacle : VARIABLE CONTROL OBSTACLE
*/
vector<fs_obstacle_info_t> fs_vec_obstacle;
```

*Ae có thể thêm các vật cản vào trong game. Ở đây mình có 3 vật cản là bom, quặng 1, quặng 2, bom, với các thông số như ID, tạo độ điểm gốc, bitmap (5x5) số điểm khi đạn bắn trúng.*

*Các bạn có thể xem giải thích kĩ hơn ở 3.1.1*

![Sequence Obstacle](https://github.com/DoVanTuan2805/_fly-and-shoot-game/blob/main/resource/images/Sequence/sequence_obstacle.png)

                                      Hình 3.3.1 Obstacle sequence

Giải thích Obstacle sequence:

**Phần 1:** Trước khi bắt đầu chuyển vào màn hình game sẽ có tín hiệu SCREEN_ENTRY khi đó sẽ xóa tất cả các quặng (nếu có). Cùng lúc đó sẽ cài đặt timer:

- Mỗi 150ms bắn message đến FS_GAME_OBSTACLE_ON_TICK_SIG
- Mỗi 800ms bắn message đến FS_GAME_OBSTACLE_PUSH_SIG.

**Phần 2:** 

- Mỗi 150ms quặng sẽ di chuyển sang phải thông qua signal FS_GAME_OBSTACLE_ON_TICK_SIG.
- Mỗi 800ms quặng 1, quặng 2, bom sẽ được tạo ra ở vị trí ngẫu nhiên ở cuối hầm nhờ vào signal FS_GAME_OBSTACLE_PUSH_SIG.

 **Phần 3:** Khi có tín hiệu FS_GAME_DISPLAY_OVER_ON_TICK sẽ xóa hết quặng (nếu có) và cài đặt lại thông số cho quặng.

Tín hiệu FS_GAME_OBSTACLE_RESET_SIG:

```cpp
// clear all obstacle
static inline void fs_game_obstacle_reset() {
		//code
}
```

Tín hiệu FS_GAME_OBSTACLE_ON_TICK:

```cpp
//  move all obstacle
static inline void fs_game_obstacle_move() {
		//code
}
```

Tín hiệu FS_GAME_OBSTACLE_PUSH_SIG:

```cpp
// add obstacle to obstacle managerment
static inline void fs_game_obstacle_push() {
		//code
}
```
**3.4. Vụ nổ (Explosion)**

Tương như các đối tượng khác trong game vụ nổ cũng có khối quản lí vụ nổ riêng

```cpp
typedef enum  {
    FS_EXPLOSION_VER_I = 0,
    FS_EXPLOSION_VER_II
} fs_ver_info_t;

typedef struct {
    fs_game_coordinate_t coordinate;
    fs_ver_info_t ver;
} fs_explosion_info_t;

vector<fs_explosion_info_t> fs_vec_explosion;
fs_explosion_info_t fs_explosion;
```

****Chi tiết hơn ae có thể xem ở mục 3.1 nhé !!!*

![Sequence Explosion](https://github.com/DoVanTuan2805/_fly-and-shoot-game/blob/main/resource/images/Sequence/sequence_explosion.png)

                                     Hình 3.5.1 Explosion Sequence

Giải thích explosion sequence:

**Phần 1:** Trước khi bắt đầu chuyển vào màn hình game sẽ có tín hiệu SCREEN_ENTRY khi đó sẽ xóa tất cả các vụ nổ (nếu có). Cùng lúc đó sẽ cài đặt timer mỗi 100ms bắn message đến FS_GAME_EXPLOSION_ON_TICK_SIG.

**Phần 2:**

- Khi có tín hiệu va chạm từ tàu bay hay của đạn thì tín hiệu FS_GAME_EXPLOSION_PUSH_SIG lập tức sẽ tạo ra vụ nổ 1 ở ngay tọa độ ở đó.
- Mỗi 100ms signal FS_GAME_EXPLOSION_ON_TICK_SIG khi đó nếu có vụ nổ thì sẽ chuyển đổi từ vụ nổ 1 sang vụ nổ 2. Khi vụ nổ 2 xảy ra thì xóa vụ nổ.

**Phần 3:** Khi có tín hiệu FS_GAME_DISPLAY_OVER_ON_TICK sẽ xóa tất cả các vụ nổ (nếu có) và đặt lại các thông số cho vụ nổ.

Tín hiệu FS_GAME_EXPLOSION_RESET_SIG:

```cpp
// clear all explosion
void fs_game_explosion_reset() {
		//code
}
```

Tín hiệu FS_GAME_EXPLOSION_ON_TICK_SIG:

```cpp
// add explsion to explosion managerment
void fs_game_explosion_push() {
		// code
}
```

Tín hiệu FS_GAME_EXPLOSION_PUSH_SIG:

```cpp
// animate explsion and erase when complete
void fs_game_explosion_update() {
		//code
}
```

**3.5. Đường hầm (Tunnel Wall)**

Tương tự như các đối tượng khác thì đường hầm cũng có biến để quản lí đường hầm.

```cpp
typedef enum { 
    FS_WALL_I = 0, 
    FS_WALL_II 
} fs_ver_wall_t;

typedef struct {
    int16_t x;
    int8_t y;
    fs_ver_wall_t ver;
} fs_wall_info_t;

/*
* fs_vec_limit_wall_y : VECTOR WALL TOP AND BOT LIMMIT MANAGERMENT
*/
vector<vector<uint8_t>> fs_vec_limit_wall_y;

/*
* fs_vec_wall : VECTOR WALL MANAGERMENT
*/
vector<fs_wall_info_t> fs_vec_wall;
```

![Sequence Tunnel Wall](https://github.com/DoVanTuan2805/_fly-and-shoot-game/blob/main/resource/images/Sequence/sequence_tunnel_wall.png)

                                        Hình 3.6.1 Tunnel Wall Sequence

Giải thích Tunnel Wall Sequence:

**Phần 1:** Trước khi bắt đầu chuyển vào màn hình game sẽ có tín hiệu SCREEN_ENTRY khi đó sẽ tạo ra 2 đường hầm liên tiếp. Cùng lúc đó sẽ cài đặt timer mỗi 150ms bắn message đến FS_GAME_WALL_ON_TICK_SIG.

**Phần 2:**

- Mỗi 150ms signal FS_GAME_WALL_ON_TICK_SIG khi đó tường sẽ di chuyển từ phải sang thái, và làm mới lại giới hạn cho đường hầm do đường hầm không bằng phẳng sẽ có những đoạn nhấp nhô.

**Phần 3:** Khi có tín hiệu FS_GAME_DISPLAY_OVER_ON_TICK sẽ xóa đường hầm nhằm giải phóng bộ nhớ.

Tín hiệu FS_GAME_WALL_SETUP_SIG:

```cpp
// set default for all wall
static inline void fs_game_wall_setup() {
		//code
}
```

Tín hiệu FS_GAME_WALL_ON_TICK_SIG:

```cpp
// move wall to left
static inline void fs_game_wall_move() {
		// code
}
```

Tín hiệu FS_GAME_WALL_RESET_SIG:

```cpp
// clear all wall
static inline void fs_game_wall_reset() {
		// code
}
```

**IV. Các hình ảnh trong game.**

Để tạo hình ảnh sinh động cho game mình sử dụng web để thiết kế các đối tượng [Photopea | Online Photo Editor](https://www.photopea.com/)  và ta cần trang web để chuyển đổi hình ảnh sang mã hex để có thể đưa vào code [image2cpp (javl.github.io)](https://javl.github.io/image2cpp/), mỗi hình ảnh được được tạo ra thành các tên biến như ở cột 4.

![IMAGE OBJECT GAME](https://github.com/DoVanTuan2805/_fly-and-shoot-game/blob/main/resource/images/image_object.png)

**Animation**:

- Vụ nổ sẽ sử dụng 2 hình ảnh một cách tuần tự để có thể tạo ra hình ảnh mang tính linh động cho game.
- Đường hầm sẽ được nối liên tiếp với nhau : Đầu bản đồ 2 sẽ được nối vào cuối bản đồ 1, khi bản đồ 1 chạy hết sẽ nối vào cuối bản đồ 2. Cứ như thế sẽ tạo ra vòng lặp vô hạn của 2 bản đồ tạo ra cảm giác cho người dùng thấy không có điểm dừng.

**V. Âm thanh của game.**

Để tạo sự sinh động cho game khi chơi. Mình có làm thêm phần âm thanh tuy không được hay nhưng cũng có thể nghe tạm được. Trang web [Online Tone Generator - generate pure tones of any frequency (szynalski.com)](https://www.szynalski.com/tone-generator/) để tạo ra code của những âm thanh. 

File code âm thanh sẽ được lưu trong *fs_buzzer_def.h*

```cpp
#include "buzzer.h"

static const Tone_TypeDef tones_explosion[] =
{
    {2000, 9},
    {0	 , 9},
    {2000, 9}
};

static const Tone_TypeDef tones_missle_push[] =
{
    {7500 , 8}
};

static const Tone_TypeDef tones_game_over[] = {
    {2000, 5},
    {0, 5},
    {3000, 5},
    {0, 5},
    {4000, 5},
    {0, 5},
    {1200, 6},
    {0, 8},
    {4500, 8},
    {2000, 3},
    {0, 3},
    {3000, 3},
    {0, 3},
    {4000, 3},
    {0, 3},
    {1200, 4},
    {0, 6},
    {4500, 6}
};
static const Tone_TypeDef tones_choose_menu[] =
{
    {2000, 5}
};

```

**Giải thích:** 

- Ví dụ: ở  “tones_missle_push” ta sẽ thấy 7500 và 8.
    - 7500 sẽ là tần số phát xung cho buzzer
    - 8(ms) sẽ là thời gian phát xung cho buzzer

(*) Phần tạo ra âm thanh này khá lằng nhằng ae nào có khả năng cảm âm được thì mới sâu được mình xin kiếu.

**VI. Sate-machines của màn hình.**

Như những phần ở trên mình có giới thiệu và giải thích từng đối tượng cho các bạn về code, các xử lí khi có event. Thì hình dưới đây sẽ xử lí event chuyển đổi các màn hình.

![STATE MACHINE SCREENS](https://github.com/DoVanTuan2805/_fly-and-shoot-game/blob/main/resource/images/state_machine_screen.png)

Ở trong game sẽ có những màn hình cụ thể như sau:

- SCREEN START-UP: Khi mới khởi động sẽ có logo.
    - Khi vào sẽ khởi tạo màn hình.
    - Đọc dữ liệu cài đặt và lịch sử của game.
    - Đặt time-out 2000ms để chuyển SCREEN MENU
- SCREEN MENU: Ở trong màn hình menu sẽ có 3 event (AC_DISPLAY_BUTTON_DOWN_PRESSED, AC_DISPLAY_BUTTON_UP_PRESSED, AC_DISPLAY_BUTTON_MODE_PRESSED)
    - Khi nhấn nút [DOWN, UP] sẽ gọi hàm *fs_switch_option_menu* để chuyển đổi các chế độ trong menu.
    - Khi nhấn nút [MODE] sẽ chuyển sang chế độ tương ứng.
    - Khi thoát khỏi màn hình menu đặt time-out 15000ms chuyển sang màn hình SCREEN IDLE.
- SCREEN GAME ON: Ở trong màn hình này. Khi vào sẽ cài đặt timer cho các đối tượng.
    - Khi nhấn nút [MODE] sẽ tạo ra event FS_GAME_MISSILE_PUSH_SIG để gọi tới hàm *fs_missile_push* có tác dụng tạo ra đạn
    - Khi nhấn nút [UP] sẽ tạo ra event FS_GAME_PLANE_UP_SIG gọi tới hàm *fs_plane_up* giúp tàu bay đi lên.
    - Khi mới vào ta có cài timer cho từng đối tượng. Thì ở phần TIME_TICK cứ mỗi 100ms sẽ tạo event FS_GAME_CRASH_SIG gọi đến hàm *fs_plane_crash* giúp kiểm tra máy bay có va chạm.
    - Khi va chạm sẽ cài time-out 2000ms bắn đến FS_GAME_DISPLAY_OVER_ON_TICK và chuyển sang màn hình SCREEN GAME OVER.
- SCREEN GAME OVER: Khi chuyển sang màn hình game over sẽ xóa tất cả các timer của đối tượn đi.
    - Khi nhấn nút [MODE] sẽ tạo ra event FS_GAME_DISPLAY_OVER_MODE_PRESSED sẽ quay lại SCREEN GAME ON và chơi tiếp.
    - Khi nhấn nút [DOWN] sẽ quay trở về SCREEN MENU.
    - Đặt time-out 15000ms nếu không có tác động gì sẽ chuyển sang SCREEN IDLE.
- SCREEN SETTING:
    - Khi nhấn nút [UP, DOWN] sẽ gọi hàm *fs_switch_option_setting* giúp chuyển đổi các chế độ của cài đặt.
    - Khi nhấn nút [MODE] sẽ cài đặt theo từng chế độ (GAME MODE, MISSILE, SOUND, EXIT)
        - GAME MODE: cài đặt độ khó cho game (EASY, NORMAL, HARD).
        - MISSILE: cài đặt số đạn tối đa cho game.
        - SOUND: bật/tắt âm thanh trong quá trình thao tác.
        - EXIT: sẽ gọi hàm *task_scr_fs_menu_handler* sẽ chuyển về SCREEN MENU.
    - Đặt time-out 15000ms nếu không thao tác sẽ chuyển sang SCREEN IDLE
- SCREEN TUTORIAL:
    - Khi nhấn [DOWN, UP, MODE] sẽ chuyển về SCREEN MENU.
    - Đặt time-out 15000ms nếu không thao tác sẽ chuyển sang SCREEN IDLE
- SCREEN EXIT:
    - Khi nhấn [MODE] sẽ chuyển sang SCREEN IDLE.
- SCREEN IDLE:
    - Khi nhấn [UP] sẽ tạo thêm bong bóng. Nếu đạt tối đa bong bóng sẽ có tiếng âm thanh.
    - Khi nhấn [DOWN] sẽ xóa bớt bong bóng. Nếu không có bong bóng sẽ có tiếng âm thanh.
    - Khi nhấn [MODE] sẽ chuyển sang SCREEN MENU.

(*) Code của các màn hình các ae có thể vào folder:  *.../game/screens.*
