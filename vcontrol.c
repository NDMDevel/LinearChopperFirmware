#include "vcontrol.h"
#include "mcc_generated_files/mcc.h"
#include "SolidStateRelay.h"
#include <stdbool.h>

//#define DUTY_RAW_MAX     379    //379 -> 95%, 399 -> 100%
#define DUTY_RAW_MAX     759    //759 -> 95%, 799 -> 100%

/*
 * ADC sampling rate: 1 KHz
 * PWM duty update rate: 1 KHz
 * PWM period: 200us
 */

//CHOPPER
static uint16_t vdc_min;
static uint16_t vdc_max;
static uint16_t vdc_critic;  //this may trigger the crowbar

static uint16_t diff_vdc;
static uint16_t vdc_prev;
static uint16_t vdc;
static uint16_t pwm_duty;
static uint16_t delta_duty;
static bool diff_positive;
static uint16_t target_duty = 0;
static uint16_t current_duty = 0;

static const uint16_t duty_pwm_inc = 8;//before it was 4
static uint16_t duty_count_up;
static uint16_t duty_count_up_max = 5;
static uint16_t duty_pwm_dec = 4;//5
static uint16_t duty_count_down;
static uint16_t duty_count_down_max = 10;
static bool chopper_active = false;
static bool init_required = true;

static const int16_t v_table[1024] = {
	  0 ,   1 ,   2 ,   2 ,   3 ,   4 ,   5 ,
	  6 ,   7 ,   7 ,   8 ,   9 ,  10 ,  11 ,
	 11 ,  12 ,  13 ,  14 ,  15 ,  16 ,  16 ,
	 17 ,  18 ,  19 ,  20 ,  21 ,  21 ,  22 ,
	 23 ,  24 ,  25 ,  25 ,  26 ,  27 ,  28 ,
	 29 ,  30 ,  30 ,  31 ,  32 ,  33 ,  34 ,
	 34 ,  35 ,  36 ,  37 ,  38 ,  39 ,  39 ,
	 40 ,  41 ,  42 ,  43 ,  44 ,  44 ,  45 ,
	 46 ,  47 ,  48 ,  48 ,  49 ,  50 ,  51 ,
	 52 ,  53 ,  53 ,  54 ,  55 ,  56 ,  57 ,
	 57 ,  58 ,  59 ,  60 ,  61 ,  62 ,  62 ,
	 63 ,  64 ,  65 ,  66 ,  67 ,  67 ,  68 ,
	 69 ,  70 ,  71 ,  71 ,  72 ,  73 ,  74 ,
	 75 ,  76 ,  76 ,  77 ,  78 ,  79 ,  80 ,
	 80 ,  81 ,  82 ,  83 ,  84 ,  85 ,  85 ,
	 86 ,  87 ,  88 ,  89 ,  89 ,  90 ,  91 ,
	 92 ,  93 ,  94 ,  94 ,  95 ,  96 ,  97 ,
	 98 ,  99 ,  99 , 100 , 101 , 102 , 103 ,
	103 , 104 , 105 , 106 , 107 , 108 , 108 ,
	109 , 110 , 111 , 112 , 112 , 113 , 114 ,
	115 , 116 , 117 , 117 , 118 , 119 , 120 ,
	121 , 122 , 122 , 123 , 124 , 125 , 126 ,
	126 , 127 , 128 , 129 , 130 , 131 , 131 ,
	132 , 133 , 134 , 135 , 135 , 136 , 137 ,
	138 , 139 , 140 , 140 , 141 , 142 , 143 ,
	144 , 145 , 145 , 146 , 147 , 148 , 149 ,
	149 , 150 , 151 , 152 , 153 , 154 , 154 ,
	155 , 156 , 157 , 158 , 158 , 159 , 160 ,
	161 , 162 , 163 , 163 , 164 , 165 , 166 ,
	167 , 167 , 168 , 169 , 170 , 171 , 172 ,
	172 , 173 , 174 , 175 , 176 , 177 , 177 ,
	178 , 179 , 180 , 181 , 181 , 182 , 183 ,
	184 , 185 , 186 , 186 , 187 , 188 , 189 ,
	190 , 190 , 191 , 192 , 193 , 194 , 195 ,
	195 , 196 , 197 , 198 , 199 , 200 , 200 ,
	201 , 202 , 203 , 204 , 204 , 205 , 206 ,
	207 , 208 , 209 , 209 , 210 , 211 , 212 ,
	213 , 213 , 214 , 215 , 216 , 217 , 218 ,
	218 , 219 , 220 , 221 , 222 , 223 , 223 ,
	224 , 225 , 226 , 227 , 227 , 228 , 229 ,
	230 , 231 , 232 , 232 , 233 , 234 , 235 ,
	236 , 236 , 237 , 238 , 239 , 240 , 241 ,
	241 , 242 , 243 , 244 , 245 , 246 , 246 ,
	247 , 248 , 249 , 250 , 250 , 251 , 252 ,
	253 , 254 , 255 , 255 , 256 , 257 , 258 ,
	259 , 259 , 260 , 261 , 262 , 263 , 264 ,
	264 , 265 , 266 , 267 , 268 , 268 , 269 ,
	270 , 271 , 272 , 273 , 273 , 274 , 275 ,
	276 , 277 , 278 , 278 , 279 , 280 , 281 ,
	282 , 282 , 283 , 284 , 285 , 286 , 287 ,
	287 , 288 , 289 , 290 , 291 , 291 , 292 ,
	293 , 294 , 295 , 296 , 296 , 297 , 298 ,
	299 , 300 , 301 , 301 , 302 , 303 , 304 ,
	305 , 305 , 306 , 307 , 308 , 309 , 310 ,
	310 , 311 , 312 , 313 , 314 , 314 , 315 ,
	316 , 317 , 318 , 319 , 319 , 320 , 321 ,
	322 , 323 , 324 , 324 , 325 , 326 , 327 ,
	328 , 328 , 329 , 330 , 331 , 332 , 333 ,
	333 , 334 , 335 , 336 , 337 , 337 , 338 ,
	339 , 340 , 341 , 342 , 342 , 343 , 344 ,
	345 , 346 , 346 , 347 , 348 , 349 , 350 ,
	351 , 351 , 352 , 353 , 354 , 355 , 356 ,
	356 , 357 , 358 , 359 , 360 , 360 , 361 ,
	362 , 363 , 364 , 365 , 365 , 366 , 367 ,
	368 , 369 , 369 , 370 , 371 , 372 , 373 ,
	374 , 374 , 375 , 376 , 377 , 378 , 379 ,
	379 , 380 , 381 , 382 , 383 , 383 , 384 ,
	385 , 386 , 387 , 388 , 388 , 389 , 390 ,
	391 , 392 , 392 , 393 , 394 , 395 , 396 ,
	397 , 397 , 398 , 399 , 400 , 401 , 402 ,
	402 , 403 , 404 , 405 , 406 , 406 , 407 ,
	408 , 409 , 410 , 411 , 411 , 412 , 413 ,
	414 , 415 , 415 , 416 , 417 , 418 , 419 ,
	420 , 420 , 421 , 422 , 423 , 424 , 424 ,
	425 , 426 , 427 , 428 , 429 , 429 , 430 ,
	431 , 432 , 433 , 434 , 434 , 435 , 436 ,
	437 , 438 , 438 , 439 , 440 , 441 , 442 ,
	443 , 443 , 444 , 445 , 446 , 447 , 447 ,
	448 , 449 , 450 , 451 , 452 , 452 , 453 ,
	454 , 455 , 456 , 457 , 457 , 458 , 459 ,
	460 , 461 , 461 , 462 , 463 , 464 , 465 ,
	466 , 466 , 467 , 468 , 469 , 470 , 470 ,
	471 , 472 , 473 , 474 , 475 , 475 , 476 ,
	477 , 478 , 479 , 480 , 480 , 481 , 482 ,
	483 , 484 , 484 , 485 , 486 , 487 , 488 ,
	489 , 489 , 490 , 491 , 492 , 493 , 493 ,
	494 , 495 , 496 , 497 , 498 , 498 , 499 ,
	500 , 501 , 502 , 502 , 503 , 504 , 505 ,
	506 , 507 , 507 , 508 , 509 , 510 , 511 ,
	512 , 512 , 513 , 514 , 515 , 516 , 516 ,
	517 , 518 , 519 , 520 , 521 , 521 , 522 ,
	523 , 524 , 525 , 525 , 526 , 527 , 528 ,
	529 , 530 , 530 , 531 , 532 , 533 , 534 ,
	535 , 535 , 536 , 537 , 538 , 539 , 539 ,
	540 , 541 , 542 , 543 , 544 , 544 , 545 ,
	546 , 547 , 548 , 548 , 549 , 550 , 551 ,
	552 , 553 , 553 , 554 , 555 , 556 , 557 ,
	558 , 558 , 559 , 560 , 561 , 562 , 562 ,
	563 , 564 , 565 , 566 , 567 , 567 , 568 ,
	569 , 570 , 571 , 571 , 572 , 573 , 574 ,
	575 , 576 , 576 , 577 , 578 , 579 , 580 ,
	580 , 581 , 582 , 583 , 584 , 585 , 585 ,
	586 , 587 , 588 , 589 , 590 , 590 , 591 ,
	592 , 593 , 594 , 594 , 595 , 596 , 597 ,
	598 , 599 , 599 , 600 , 601 , 602 , 603 ,
	603 , 604 , 605 , 606 , 607 , 608 , 608 ,
	609 , 610 , 611 , 612 , 613 , 613 , 614 ,
	615 , 616 , 617 , 617 , 618 , 619 , 620 ,
	621 , 622 , 622 , 623 , 624 , 625 , 626 ,
	626 , 627 , 628 , 629 , 630 , 631 , 631 ,
	632 , 633 , 634 , 635 , 636 , 636 , 637 ,
	638 , 639 , 640 , 640 , 641 , 642 , 643 ,
	644 , 645 , 645 , 646 , 647 , 648 , 649 ,
	649 , 650 , 651 , 652 , 653 , 654 , 654 ,
	655 , 656 , 657 , 658 , 658 , 659 , 660 ,
	661 , 662 , 663 , 663 , 664 , 665 , 666 ,
	667 , 668 , 668 , 669 , 670 , 671 , 672 ,
	672 , 673 , 674 , 675 , 676 , 677 , 677 ,
	678 , 679 , 680 , 681 , 681 , 682 , 683 ,
	684 , 685 , 686 , 686 , 687 , 688 , 689 ,
	690 , 691 , 691 , 692 , 693 , 694 , 695 ,
	695 , 696 , 697 , 698 , 699 , 700 , 700 ,
	701 , 702 , 703 , 704 , 704 , 705 , 706 ,
	707 , 708 , 709 , 709 , 710 , 711 , 712 ,
	713 , 714 , 714 , 715 , 716 , 717 , 718 ,
	718 , 719 , 720 , 721 , 722 , 723 , 723 ,
	724 , 725 , 726 , 727 , 727 , 728 , 729 ,
	730 , 731 , 732 , 732 , 733 , 734 , 735 ,
	736 , 737 , 737 , 738 , 739 , 740 , 741 ,
	741 , 742 , 743 , 744 , 745 , 746 , 746 ,
	747 , 748 , 749 , 750 , 750 , 751 , 752 ,
	753 , 754 , 755 , 755 , 756 , 757 , 758 ,
	759 , 759 , 760 , 761 , 762 , 763 , 764 ,
	764 , 765 , 766 , 767 , 768 , 769 , 769 ,
	770 , 771 , 772 , 773 , 773 , 774 , 775 ,
	776 , 777 , 778 , 778 , 779 , 780 , 781 ,
	782 , 782 , 783 , 784 , 785 , 786 , 787 ,
	787 , 788 , 789 , 790 , 791 , 792 , 792 ,
	793 , 794 , 795 , 796 , 796 , 797 , 798 ,
	799 , 800 , 801 , 801 , 802 , 803 , 804 ,
	805 , 805 , 806 , 807 , 808 , 809 , 810 ,
	810 , 811 , 812 , 813 , 814 , 815 , 815 ,
	816 , 817 , 818 , 819 , 819 , 820 , 821 ,
	822 , 823 , 824 , 824 , 825 , 826 , 827 ,
	828 , 828 , 829 , 830 , 831 , 832 , 833 ,
	833 , 834 , 835 , 836 , 837 , 837 , 838 ,
	839 , 840 
};

static enum
{
    SHUTDOWN,
    ACTIVE

}st = SHUTDOWN;

static void set_vdc_threshold(uint16_t v_val,uint16_t *v_target)
{
    if( v_val == 0xFFFF )
        return;
    if( v_val >= VDC_MAX )
        *v_target = VDC_MAX;
    else
        *v_target = v_val;
}

static void LoadDutyValue(uint16_t duty)
{
    target_duty = duty;
}

void init_chopper()
{
    if( init_required == false )
        return;
    init_required = false;
    ADC_SelectChannel(V_BUSDC);
    __delay_ms(1);          //wait ADC to switch conversion channel
    ADC_StartConversion();
    TMR2_StartTimer();

    TMR2_SetInterruptHandler(TMR2_DutyControlHandler_ISR);
    ADC_SetInterruptHandler(ADC_VoltageControlHandler_ISR);
}

void start_chopper()
{
    if( chopper_active == true )
        return;
    LED_G_TRIS = 0;
    LED_R_TRIS = 0;
    pwm_duty = 0;
    current_duty = 0;
    target_duty = 0;

    if( init_required == true )
        init_chopper();
    
    chopper_active = true;
}

void stop_chopper()
{
    if( chopper_active == false )
        return;

    LED_G_TRIS = 1;
    LED_R_TRIS = 1;
    target_duty = 0;
    chopper_active = false;
//    ADC_SetInterruptHandler(ADC_DefaultInterruptHandler);
}

bool is_chopper_active()
{
    return chopper_active;
}

void set_vdc_min(uint16_t vmin)
{
    set_vdc_threshold(vmin,&vdc_min);
}

void set_vdc_max(uint16_t vmax)
{
    set_vdc_threshold(vmax,&vdc_max);
}

void set_vdc_critic(uint16_t vc)
{
    set_vdc_threshold(vc,&vdc_critic);
}

void set_vdc_speed(uint16_t msDelay)
{
    if( msDelay == 0xFFFF )
        return;
    if( msDelay > 25500 )
        duty_count_up_max = 255;
    duty_count_up_max = msDelay/100 - 1;
}

void save_to_flash(void)
{
    uint16_t buff[WRITE_FLASH_BLOCKSIZE];
    if( chopper_active == true )
        buff[0] = CHOPPER_ENABLED;
    else
        buff[0] = CHOPPER_DISABLED;
    buff[1] = vdc_min;
    buff[2] = vdc_max;
    buff[3] = vdc_critic;
    buff[4] = duty_count_up_max;
    buff[5] = get_relay_reset_voltage();
    buff[6] = get_reset_duration();
    FLASH_WriteBlock(FLASH_START_ADDRESS,buff);
}

uint16_t get_vdc_min(void)
{
    return vdc_min;
}

uint16_t get_vdc_max(void)
{
    return vdc_max;
}

uint16_t get_vdc_critic(void)
{
    return vdc_critic;
}

uint16_t get_vdc_speed(void)
{
    //returns the speed in mili-seconds
    return (duty_count_up_max + 1)*100;
}

uint16_t get_vdc(void)
{
    return vdc;
}

void ADC_VoltageControlHandler_ISR(void)
{
    vdc_prev    = vdc;
    vdc         = v_table[ADC_GetConversionResult() & 0x03FF ];

    if( chopper_active == false )
        return;
    
    diff_vdc    = vdc - vdc_prev;
    if( vdc > vdc_prev )
        diff_positive = true;
    else
        diff_positive = false;

    if( vdc > vdc_critic )
    {
        LED_R_SetHigh();
        LED_G_SetLow();
        PWM3_LoadDutyValue(DUTY_RAW_MAX);
        current_duty = DUTY_RAW_MAX;
        target_duty = DUTY_RAW_MAX;
        pwm_duty = DUTY_RAW_MAX;
    }
    else
    {
        if( vdc >= vdc_max )
        {
            //Red on, Green off
            LED_R_SetHigh();
            LED_G_SetLow();
            pwm_duty = DUTY_RAW_MAX;
        }
        else
        {
            if( vdc >= vdc_min )
            {
                LED_R_SetHigh();
                LED_G_SetHigh();
                bool force_inc = true;
                if( diff_positive == true )
                {
                    uint32_t pwm = (uint32_t)DUTY_RAW_MAX * (uint32_t)(vdc - vdc_min);
                    pwm /= (vdc_max - vdc_min);
                    if( pwm > DUTY_RAW_MAX )
                    {
                        pwm_duty = DUTY_RAW_MAX;
                        force_inc = false;
                    }
                    else
                    {
                        if( pwm_duty < pwm )
                        {
                            pwm_duty = pwm;
                            force_inc = false;
                        }
                    }
                }
                if( force_inc == true )
                {
                    if( duty_count_up >= duty_count_up_max )
                    {
                        duty_count_up = 0;
                        pwm_duty += duty_pwm_inc;
                        if( pwm_duty > DUTY_RAW_MAX )
                            pwm_duty = DUTY_RAW_MAX;
                    }
                    else
                        duty_count_up++;
                }
                duty_count_down = 0;
            }
            else
            {
                LED_R_SetLow();
                LED_G_SetHigh();
                if( duty_count_down >= duty_count_down_max )
                {
                    duty_count_down = 0;
                    if( pwm_duty > duty_pwm_dec )
                        pwm_duty -= duty_pwm_dec;
                    else
                        pwm_duty = 0;
                }
                else
                    duty_count_down++;
            }
        }
    }
//    if( pwm_duty > DUTY_RAW_MAX )
//        pwm_duty = DUTY_RAW_MAX;
    LoadDutyValue( pwm_duty );
}

//40 -> 5% duty inc: 0->100% in  20ms
//8  -> 1& duty inc: 0->100% in 100ms
static const uint16_t duty_inc = 8;

void TMR2_DutyControlHandler_ISR(void)
{
    if( current_duty > target_duty )
    {
        if( current_duty - target_duty > duty_inc )
            current_duty -= duty_inc;
        else
            current_duty = target_duty;
    }
    if( current_duty < target_duty )
    {
        if( target_duty - current_duty > duty_inc  )
//        if( current_duty + duty_inc <= target_duty )
            current_duty += duty_inc;
        else
            current_duty = target_duty;
    }
    PWM3_LoadDutyValue(current_duty);
}
