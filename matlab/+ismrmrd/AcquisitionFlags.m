classdef AcquisitionFlags
    properties(Constant)
        % Looping indicators %
        ACQ_FIRST_IN_ENCODE_STEP1                	= 1,
        ACQ_LAST_IN_ENCODE_STEP1    				= 2,
        ACQ_FIRST_IN_ENCODE_STEP2   				= 3,
        ACQ_LAST_IN_ENCODE_STEP2    				= 4,
        ACQ_FIRST_IN_AVERAGE        				= 5,
        ACQ_LAST_IN_AVERAGE         				= 6,
        ACQ_FIRST_IN_SLICE          				= 7,
        ACQ_LAST_IN_SLICE           				= 8,
        ACQ_FIRST_IN_CONTRAST       				= 9,
        ACQ_LAST_IN_CONTRAST        				= 10,
        ACQ_FIRST_IN_PHASE          				= 11,
        ACQ_LAST_IN_PHASE           				= 12,
        ACQ_FIRST_IN_REPETITION     				= 13,
        ACQ_LAST_IN_REPETITION      				= 14,
        ACQ_FIRST_IN_SET            				= 15,
        ACQ_LAST_IN_SET             				= 16,
        ACQ_FIRST_IN_SEGMENT        				= 17,
        ACQ_LAST_IN_SEGMENT         				= 18,

        ACQ_IS_NOISE_MEASUREMENT                	= 19,
        ACQ_IS_PARALLEL_CALIBRATION             	= 20,
        ACQ_IS_PARALLEL_CALIBRATION_AND_IMAGING 	= 21,
        ACQ_IS_REVERSE              				= 22,
        ACQ_IS_NAVIGATION_DATA      				= 23,

        ACQ_USER1                   				= 57,
        ACQ_USER2                   				= 58,
        ACQ_USER3                   				= 59,
        ACQ_USER4                   				= 60,
        ACQ_USER5                   				= 61,
        ACQ_USER6                   				= 62,
        ACQ_USER7                   				= 63,
        ACQ_USER8                   				= 64
    end
end