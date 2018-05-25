classdef WaveformHeader < handle
    
    properties
        
        version = uint16([]);                          % First unsigned int indicates the version %
        flags = uint64([]);                            % bit field with flags %
        measurement_uid = uint32([]);                  % Unique ID for the measurement %
        scan_counter = uint32([]);                     % Current acquisition number in the measurement %
        time_stamp = uint32([]);            % Physiology time stamps, e.g. ecg, breating, etc. %
        number_of_samples = uint16([]);                % Number of samples acquired %
        channels = uint16([]);               % Available channels %
        sample_time_us = single([]);                   % Time between samples in micro seconds, sampling BW %
        waveform_id = uint16([]);

        
    end
      
    methods
        
        function obj = WaveformHeader(arg)
            % Constructor
            
            switch nargin
                case 0
                    % No argument constructor
                    % initialize to a single acquisition header
                    extend(obj,1);
                    
                case 1
                    % One argument constructor
                    if isstruct(arg)
                        % plain struct
                        fromStruct(obj,arg);
                    elseif (length(arg)==1 && ismrmrd.util.isInt(arg)) == 1
                        % number
                        extend(obj,arg);
                    elseif isa(arg,'uint8')
                        % Byte array
                        fromBytes(obj,arg);
                    else
                        % Unknown type
                        error('Unknown argument type.')
                    end
                    
                otherwise
                    error('Wrong number of arguments.')
                    
            end
        end
        
        function nacq = getNumber(obj)
            % Return the number of headers
            
            nacq = length(obj.version);
            
        end

        function hdr = select(obj, range)
            % Return a copy of a range of acquisition headers
            
            % create an empty acquisition header
            M = length(range);
            hdr = ismrmrd.AcquisitionHeader(M);
            
            % Fill
            hdr.version = obj.version(range);
            hdr.flags = obj.flags(range);
            hdr.measurement_uid = obj.measurement_uid(range);
            hdr.scan_counter = obj.scan_counter(range);
            hdr.time_stamp= obj.time_stamp(range);
            hdr.number_of_samples = obj.number_of_samples(range);
            hdr.channels = obj.channels(range);
            hdr.sample_time_us = obj.sample_time_us(range);
            hdr.waveform_id = obj.waveform_id(range);
            
        end        
        
        function extend(obj,N)
            % Extend with blank header
            
            range = obj.getNumber + (1:N);
            obj.version(1,range)                  = zeros(1,N,'uint16');
            obj.flags(1,range)                    = zeros(1,N,'uint64');
            obj.measurement_uid(1,range)          = zeros(1,N,'uint32');
            obj.scan_counter(1,range)             = zeros(1,N,'uint32');
            obj.time_stamp(1,range)               = zeros(1,N,'uint32');
            obj.number_of_samples(1,range)        = zeros(1,N,'uint16');
            obj.channels(1,range)                 = zeros(1,N,'uint16');
            obj.channels(1,range)                 = zeros(1,N,'uint16');
            obj.sample_time_us(1,range)           = zeros(1,N,'single');
            obj.waveform_id(1,range)              = zeros(1,N,'uint16');
        end
        
        function append(obj, head)
            % Append a header
            
            Nstart = obj.getNumber + 1;
            Nend   = obj.getNumber + length(head.version);
            Nrange = Nstart:Nend;
            obj.version(1,Nrange) = hdr.version(:);
            obj.flags(1,Nrange) = hdr.flags(:);
            obj.measurement_uid(1,Nrange) = hdr.measurement_uid(:);
            obj.scan_counter(1,Nrange) = hdr.scan_counter(:);
            obj.time_stamp(1,Nrange) = hdr.time_stamp(:);
            obj.number_of_samples(1,Nrange) = hdr.number_of_samples(:);
            obj.channels(1,Nrange) = hdr.channels(:);
            obj.sample_time_us(1,Nrange) = hdr.sample_time_us(:);
            obj.waveform_id(1,Nrange) = hdr.waveform_id(:);
                    
        end

        function fromStruct(obj, hdr)
            % Convert a struct to the object
            N = length(hdr.version);
            %warning! no error checking
            obj.version = reshape(hdr.version,[1,N]);
            obj.flags = reshape(hdr.flags,[1,N]);
            obj.measurement_uid = reshape(hdr.measurement_uid,[1,N]);
            obj.scan_counter = reshape(hdr.scan_counter,[1,N]);
            obj.time_stamp = reshape(hdr.time_stamp,[1,N]);
            obj.number_of_samples = reshape(hdr.number_of_samples,[1,N]);
            obj.channels = reshape(hdr.channels,[1,N]);
            obj.sample_time_us = reshape(hdr.sample_time_us,[1,N]);
            obj.waveform_id = reshape(hdr.waveform_id, [1,N]);
        end

        function hdr = toStruct(obj)
            % Convert the object to a plain struct
            
            %warning! no error checking
            hdr = struct();
            hdr.version = obj.version;
            hdr.flags = obj.flags;
            hdr.measurement_uid = obj.measurement_uid;
            hdr.scan_counter = obj.scan_counter;
            hdr.time_stamp = obj.time_stamp;
            hdr.number_of_samples = obj.number_of_samples;
            hdr.channels = obj.channels;
            hdr.sample_time_us = obj.sample_time_us;
            hdr.waveform_id = obj.waveform_id;

        end
        
        function fromBytes(obj, bytearray)
            % Convert from a byte array to an ISMRMRD AcquisitionHeader
            % This conforms to the memory layout of the C-struct

            if size(bytearray,1) ~= 38 
                error('Wrong number of bytes for AcquisitionHeader.')
            end
            N = size(bytearray,2);

            obj.version =                  reshape(typecast(reshape(bytearray(  1:   2, :), 1, 2 *      N), 'uint16'),  1, N);  % First unsigned int indicates the version %
            obj.flags =                    reshape(typecast(reshape(bytearray(  9:  16, :), 1, 8 *      N), 'uint64'),  1, N);  % bit field with flags %
            obj.measurement_uid =          reshape(typecast(reshape(bytearray(  17: 20, :), 1, 4 *      N), 'uint32'),  1, N);  % Unique ID for the measurement %
            obj.scan_counter =             reshape(typecast(reshape(bytearray(  21: 24, :), 1, 4 *      N), 'uint32'),  1, N);  % Current acquisition number in the measurement %
            obj.acquisition_time_stamp =   reshape(typecast(reshape(bytearray(  25: 28, :), 1, 4 *      N), 'uint32'),  1, N);  % Acquisition clock %
            obj.number_of_samples =        reshape(typecast(reshape(bytearray(  29: 30, :), 1, 2 *      N), 'uint16'),  1, N);  % Number of samples acquired %
            obj.channels =                 reshape(typecast(reshape(bytearray(  31 : 32, :), 1, 2 *      N), 'uint16'),  1, N);  % Available channels%
            obj.sample_time_us =           reshape(typecast(reshape(bytearray(  33: 36, :), 1, 2 *      N), 'single'),  1, N);  % Sample time in micro seconds %
            obj.waveform_id =              reshape(typecast(reshape(bytearray(  37: 38, :), 1, 2 *      N), 'uint16'),  1, N);  % Waveform ID %


        end
        
        function bytes = toBytes(obj)
            % Convert to an ISMRMRD AcquisitionHeader to a byte array
            % This conforms to the memory layout of the C-struct

            N = obj.getNumber;
            bytes = zeros(38,N,'uint8');
            for p = 1:N
                off = 1;
                bytes(off:off+1,p)   = typecast(obj.version(p)               ,'uint8'); off=off+8;
                bytes(off:off+7,p)   = typecast(obj.flags(p)                 ,'uint8'); off=off+8;
                bytes(off:off+3,p)   = typecast(obj.measurement_uid(p)       ,'uint8'); off=off+4;
                bytes(off:off+3,p)   = typecast(obj.scan_counter(p)          ,'uint8'); off=off+4;
                bytes(off:off+3,p)   = typecast(obj.time_stamp(p)            ,'uint8'); off=off+4;
                bytes(off:off+1,p)   = typecast(obj.number_of_samples(p)     ,'uint8'); off=off+2;
                bytes(off:off+1,p)   = typecast(obj.channels(p)              ,'uint8'); off=off+2;
                bytes(off:off+3,p)   = typecast(obj.sample_time_us(p)        ,'uint8'); off=off+4;
                bytes(off:off+1,p)   = typecast(obj.waveform_id(p)           ,'uint8'); off=off+2;
            end
        end
        
              
        function ret = flagIsSet(obj, flag, range)
            % bool = obj.flagIsSet(flag, range)
            
            if nargin < 3
                range = 1:obj.getNumber;
            end
            if isa(flag, 'char')
                b = obj.FLAGS.(flag);
            elseif (flag>0)
                b = uint64(flag);
            else
                error('Flag is of the wrong type.'); 
            end
            bitmask = bitshift(uint64(1),(b-1));
            ret = zeros(size(range));
            for p = 1:length(range)
                ret(p) = (bitand(obj.flags(range(p)), bitmask)>0);
            end
        end
        
        function flagSet(obj, flag, range)
            if nargin < 3
                range = 1:obj.getNumber;
            end
            if isa(flag, 'char')
                b = obj.FLAGS.(flag);
            elseif (flag>0)
                b = uint64(flag);
            else
                error('Flag is of the wrong type.'); 
            end
            bitmask = bitshift(uint64(1),(b-1));

            alreadyset = obj.flagIsSet(flag,range);
            for p = 1:length(range)
                if ~alreadyset(p)
                    obj.flags(range(p)) = obj.flags(range(p)) + bitmask;
                end
            end
        end
        
        function flagClear(obj, flag, range)
            if nargin < 3
                range = 1:obj.getNumber;
            end
            
            if isa(flag, 'char')
                b = obj.FLAGS.(flag);
            elseif (flag>0)
                b = uint64(flag);
            else
                error('Flag is of the wrong type.'); 
            end
            bitmask = bitshift(uint64(1),(b-1));
            
            alreadyset = obj.flagIsSet(flag,range);
            for p = 1:length(range)
                if alreadyset(p)
                    obj.flags(range(p)) = obj.flags(range(p)) - bitmask;
                end
            end
        end
        
        function flagClearAll(obj, range)
            if nargin < 2
                range = 1:obj.getNumber;
            end
            obj.flags(range) = zeros(1,length(range),'uint64');
        end
        
    end
    
end
