#include <stdint.h>
#include "PLL.h"
#include "SysTick.h"
#include "UART0.h"
#include "Servo.h"
#include "HCSR04.h"

// hardware and physical constraints
#define MAX_DISTANCE_CM         40
#define TRACKING_THRESHOLD_CM   30
#define SERVO_SETTLE_TIME_MS    40

// sensor sweep configuration
#define MIN_ANGLE_DEG           0
#define MAX_ANGLE_DEG           180
#define ANGLE_STEP_DEG          5
// calculate memory size automatically: (180 / 5) + 1 = 37 memory slots
#define MAP_SIZE                ((MAX_ANGLE_DEG / ANGLE_STEP_DEG) + 1) 
#define EMPTY_CELL              999

// grid configuration
#define Y_AXIS_STEPS            8
#define DISTANCE_STEP_CM        5

// ansi escape codes for tera term
#define CLEAR_SCREEN            "\033[2J"
#define CURSOR_HOME             "\033[H"
#define HIDE_CURSOR             "\033[?25l"
#define COLOR_GREEN             "\033[32m"
#define COLOR_RED               "\033[31m"

int main(void) {
    // hardware initialization
    PLL_Init(); 
    SysTick_Init(); 
    UART0_Init(); 
    Servo_Init(); 
    HCSR04_Init();

    // state variables
    int16_t  current_angle = MIN_ANGLE_DEG;
    int8_t   sweep_direction = ANGLE_STEP_DEG; 
    uint32_t measured_distance = 0;
    
    // occupancy grid array to store physical map data
    uint32_t occupancy_grid[MAP_SIZE];
    
    // initialize memory array with empty values
    for(int i = 0; i < MAP_SIZE; i++) {
        occupancy_grid[i] = EMPTY_CELL; 
    }

    // initialize terminal display
    UART0_Output_String(CLEAR_SCREEN); 
    UART0_Output_String(HIDE_CURSOR);

    while(1) {
        // actuate and sense
        Servo_SetAngle(current_angle);
        SysTick_Wait1ms(SERVO_SETTLE_TIME_MS); 
        
        measured_distance = HCSR04_GetDistance();
        
        // calculate the array index corresponding to the current physical angle
        uint8_t grid_index = current_angle / ANGLE_STEP_DEG;
        
        // update occupancy grid: record the wall, or actively clear empty space
        if (measured_distance > 0 && measured_distance <= MAX_DISTANCE_CM) {
            occupancy_grid[grid_index] = measured_distance; 
        } else {
            occupancy_grid[grid_index] = EMPTY_CELL; 
        }

        // render terminal user interface via frame buffer
        UART0_Output_String(CURSOR_HOME);
        UART0_Output_String(COLOR_GREEN);
        UART0_Output_String("=== MAPPING SONAR ===\r\n\n");
        
        // render distance y-axis and map grid
        for(int grid_row = Y_AXIS_STEPS; grid_row >= 1; grid_row--) {
            uint32_t row_max_dist = grid_row * DISTANCE_STEP_CM;
            uint32_t row_min_dist = row_max_dist - DISTANCE_STEP_CM;
            
            // print y-axis labels (padded for single digits)
            if (row_max_dist < 10) {
                UART0_Output_Character(' ');
            }
            UART0_Output_Unsigned_Decimal(row_max_dist); 
            UART0_Output_String(" +");
            
            // render the columns for this specific row
            for(int grid_col = 0; grid_col < MAP_SIZE; grid_col++) {
                
                // check if recorded memory falls within this specific block
                if(occupancy_grid[grid_col] <= row_max_dist && occupancy_grid[grid_col] > row_min_dist) {
                    UART0_Output_String(COLOR_RED);
                    if (grid_col == grid_index) {
                        UART0_Output_Character('X'); // live target under active scan
                    } else {
                        UART0_Output_Character('#'); // stored memory target
                    }
                    UART0_Output_String(COLOR_GREEN);
                } 
                // draw sweeping vertical cursor
                else if (grid_col == grid_index) {
                    UART0_Output_Character('|');
                } 
                // empty space
                else {
                    UART0_Output_Character('.');
                }
            }
            UART0_Output_String("\r\n");
        }
        
        // render angle x-axis
        UART0_Output_String("   +-------------------------------------+\r\n");
        UART0_Output_String("    0      45     90    135    180\r\n");

        // seek and track evaluation
        if (measured_distance > 0 && measured_distance < TRACKING_THRESHOLD_CM) {
            // target is too close: invert direction immediately to jitter and track
            sweep_direction = -sweep_direction; 
        } else {
            // standard wide scan
            current_angle += sweep_direction;
            
            // keep servo within physical constraints
            if(current_angle >= MAX_ANGLE_DEG || current_angle <= MIN_ANGLE_DEG) {
                sweep_direction = -sweep_direction;
            }
        }
    }
}