# V. Âm thanh của game

[Quay lại README](../README.md)

Âm thanh tạo sự sinh động khi chơi. Sử dụng [Online Tone Generator](https://www.szynalski.com/tone-generator/) để tạo code của các âm thanh.

File code âm thanh được lưu trong `fs_buzzer_def.h`.

```cpp
#include "buzzer.h"

static const Tone_TypeDef tones_explosion[] =
{
    {2000, 9},
    {0    , 9},
    {2000, 9}
};

static const Tone_TypeDef tones_missle_push[] =
{
    {7500 , 8}
};
```

**Giải thích:**

- Ví dụ ở `tones_missle_push` ta thấy 7500 và 8.
    - 7500 là tần số phát xung cho buzzer.
    - 8 (ms) là thời gian phát xung cho buzzer.

[Quay lại README](../README.md)
