# FLY AND SHOOT Game trên AK-Embedded Base Kit

![BANNER GAME](resource/images/banner_game.png)

# Giới thiệu

Trò chơi "Fly and Shoot" là tựa game kinh điển được tái tạo trên nền tảng AK-Embedded Base Kit, tích hợp hệ thống quản lý nhiệm vụ, tín hiệu, bộ đếm thời gian và tin nhắn kết hợp với cơ chế Event-Driven, giúp game phản ứng linh hoạt theo sự kiện và tạo trải nghiệm chơi mượt mà, hấp dẫn cho người chơi.

Code các màn hình nằm trong folder `application/sources/app/game/screens/`.

# Mục lục

| Mục | Nội dung |
| --- | --- |
| [I. Giới thiệu](docs/01-gioi-thieu.md) | Phần cứng AK Kit, menu, các đối tượng, cách chơi và tính điểm |
| [II. Task và Signal](docs/02-task-signal.md) | Event-Driven, các task, signal và sequence trong game |
| [III. Chi tiết code các đối tượng](docs/03-doi-tuong.md) | Plane, Missile, Obstacle, Explosion, Tunnel Wall |
| [IV. Các hình ảnh trong game](docs/04-hinh-anh.md) | Workflow tạo ảnh và animation |
| [V. Âm thanh của game](docs/05-am-thanh.md) | Tone và file `fs_buzzer_def.h` |
| [VI. State-machine các màn hình](docs/06-state-machine.md) | Chuyển đổi giữa các màn hình |

---
