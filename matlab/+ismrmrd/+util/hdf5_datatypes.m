classdef hdf5_datatypes
% This convenience class defines the HDF5 types used in the
% ISMRMRD HDF5 file

% The names, types, layout and offsets consistent with that generated
% by the C++ API.  See the note at the bottom of the file for how to
% do this.

    properties
        T_float;
        T_double;
        T_char;
        T_complexfloat;
        T_complexdouble;
        T_ushort;
        T_EncodingCounters;
        T_AcquisitionHeader;
        T_Acquisition;
    end
    
    methods
        
        function obj = hdf5_datatypes()
            obj.T_float = ismrmrd.util.hdf5_datatypes.getType_float();
            obj.T_double = ismrmrd.util.hdf5_datatypes.getType_double();
            obj.T_char = ismrmrd.util.hdf5_datatypes.getType_char();
            obj.T_complexfloat = ismrmrd.util.hdf5_datatypes.getType_complexfloat();
            obj.T_complexdouble = ismrmrd.util.hdf5_datatypes.getType_complexdouble();
            obj.T_ushort = ismrmrd.util.hdf5_datatypes.getType_ushort();
            obj.T_EncodingCounters = ismrmrd.util.hdf5_datatypes.getType_EncodingCounters();
            obj.T_AcquisitionHeader = ismrmrd.util.hdf5_datatypes.getType_AcquisitionHeader();
            obj.T_Acquisition = ismrmrd.util.hdf5_datatypes.getType_Acquisition();
        end
        
    end
   
    methods (Static)

        function b = getType_float()
            b = H5T.copy('H5T_NATIVE_FLOAT');
        end

        function b = getType_double()
            b = H5T.copy('H5T_NATIVE_DOUBLE');
        end

        function b = getType_char()
            b = H5T.copy('H5T_NATIVE_CHAR');
        end

        function b = getType_complexfloat()
            typesize = 2*H5T.get_size('H5T_NATIVE_FLOAT');
            b = H5T.create ('H5T_COMPOUND', typesize);
            H5T.insert (b, 'real', 0, 'H5T_NATIVE_FLOAT');
            H5T.insert (b, 'imag', H5T.get_size('H5T_NATIVE_FLOAT'), 'H5T_NATIVE_FLOAT');
        end

        function b = getType_complexdouble()
            b = H5T.create ('H5T_COMPOUND', ...
                             2*H5T.get_size('H5T_NATIVE_DOUBLE'));
            H5T.insert (b, 'real', 0, 'H5T_NATIVE_DOUBLE');
            H5T.insert (b, 'imag', H5T.get_size('H5T_NATIVE_DOUBLE'), 'H5T_NATIVE_DOUBLE');
        end

        function b = getType_ushort()
            b = H5T.copy('H5T_NATIVE_USHORT');
        end

        function b = getType_EncodingCounters()

            b = H5T.create ('H5T_COMPOUND', 34);
            H5T.insert(b, 'kspace_encode_step_1', 0, 'H5T_NATIVE_UINT16');
            H5T.insert(b, 'kspace_encode_step_2', 2, 'H5T_NATIVE_UINT16');
            H5T.insert(b, 'average', 4, 'H5T_NATIVE_UINT16');
            H5T.insert(b, 'slice', 6, 'H5T_NATIVE_UINT16');
            H5T.insert(b, 'contrast', 8, 'H5T_NATIVE_UINT16');
            H5T.insert(b, 'phase', 10, 'H5T_NATIVE_UINT16');
            H5T.insert(b, 'repetition', 12, 'H5T_NATIVE_UINT16');
            H5T.insert(b, 'set', 14, 'H5T_NATIVE_UINT16');
            H5T.insert(b, 'segment', 16, 'H5T_NATIVE_UINT16');
            H5T.insert(b, 'user', 18, H5T.array_create('H5T_NATIVE_UINT16',[8]));

        end

        function b = getType_AcquisitionHeader()
            b = H5T.create ('H5T_COMPOUND', 360);
            H5T.insert(b, 'version', 0, 'H5T_NATIVE_UINT16');
            H5T.insert(b, 'flags', 2, 'H5T_NATIVE_UINT64');
            H5T.insert(b, 'measurement_uid', 10, 'H5T_NATIVE_UINT32');
            H5T.insert(b, 'scan_counter', 14, 'H5T_NATIVE_UINT32');
            H5T.insert(b, 'acquisition_time_stamp', 18, 'H5T_NATIVE_UINT32');
            H5T.insert(b, 'physiology_time_stamp', 22, H5T.array_create('H5T_NATIVE_UINT32',[3]));
            H5T.insert(b, 'number_of_samples', 54, 'H5T_NATIVE_UINT16');
            H5T.insert(b, 'available_channels', 56, 'H5T_NATIVE_UINT16');
            H5T.insert(b, 'active_channels', 58, 'H5T_NATIVE_UINT16');
            H5T.insert(b, 'channel_mask', 60, H5T.array_create('H5T_NATIVE_UINT64',[16]));
            H5T.insert(b, 'discard_pre', 188, 'H5T_NATIVE_UINT16');
            H5T.insert(b, 'discard_post', 190, 'H5T_NATIVE_UINT16');
            H5T.insert(b, 'center_sample', 192, 'H5T_NATIVE_UINT16');
            H5T.insert(b, 'encoding_space_ref', 194, 'H5T_NATIVE_UINT16');
            H5T.insert(b, 'trajectory_dimensions', 196, 'H5T_NATIVE_UINT16');
            H5T.insert(b, 'sample_time_us', 198, 'H5T_NATIVE_FLOAT');
            H5T.insert(b, 'position', 202, H5T.array_create('H5T_NATIVE_FLOAT',[3]));
            H5T.insert(b, 'read_dir', 214, H5T.array_create('H5T_NATIVE_FLOAT',[3]));
            H5T.insert(b, 'phase_dir', 226, H5T.array_create('H5T_NATIVE_FLOAT',[3]));
            H5T.insert(b, 'slice_dir', 238, H5T.array_create('H5T_NATIVE_FLOAT',[3]));
            H5T.insert(b, 'patient_table_position', 250, H5T.array_create('H5T_NATIVE_FLOAT',[3]));
            H5T.insert(b, 'idx', 262, ismrmrd.util.hdf5_datatypes.getType_EncodingCounters);
            H5T.insert(b, 'user_int', 296, H5T.array_create('H5T_NATIVE_INT32',[8]));
            H5T.insert(b, 'user_float', 328, H5T.array_create('H5T_NATIVE_FLOAT',[8]));
        end

        function b = getType_Acquisition()

            head = H5T.copy(ismrmrd.util.hdf5_datatypes.getType_AcquisitionHeader());
            traj = H5T.vlen_create(ismrmrd.util.hdf5_datatypes.getType_float());
            data = H5T.vlen_create(ismrmrd.util.hdf5_datatypes.getType_float());

            b = H5T.create ('H5T_COMPOUND', 392);
            H5T.insert(b, 'head', 0, head);
            H5T.insert(b, 'traj', 360, traj);
            H5T.insert(b, 'data', 376, data);

        end
    end % Methods (Static)

end

% Generate a dataset using the C++ utilities and run
% h5ls -a -v testdata/dataset/data
% This produces something the following output
%"head"             +0    struct {
%  "version"          +0    native unsigned short
%  "flags"            +2    native unsigned long
%  "measurement_uid"  +10   native unsigned int
%  "scan_counter"     +14   native unsigned int
%  "acquisition_time_stamp" +18   native unsigned int
%  "physiology_time_stamp" +22   [3] native unsigned int
%  "number_of_samples" +54   native unsigned short
%  "available_channels" +56   native unsigned short
%  "active_channels"  +58   native unsigned short
%  "channel_mask"     +60   [16] native unsigned long
%  "discard_pre"      +188  native unsigned short
%  "discard_post"     +190  native unsigned short
%  "center_sample"    +192  native unsigned short
%  "encoding_space_ref" +194  native unsigned short
%  "trajectory_dimensions" +196  native unsigned short
%  "sample_time_us"   +198  native float
%  "position"         +202  [3] native float
%  "read_dir"         +214  [3] native float
%  "phase_dir"        +226  [3] native float
%  "slice_dir"        +238  [3] native float
%  "patient_table_position" +250  [3] native float
%  "idx"              +262  struct {
%     "kspace_encode_step_1" +0    native unsigned short
%     "kspace_encode_step_2" +2    native unsigned short
%     "average"          +4    native unsigned short
%     "slice"            +6    native unsigned short
%     "contrast"         +8    native unsigned short
%     "phase"            +10   native unsigned short
%     "repetition"       +12   native unsigned short
%     "set"              +14   native unsigned short
%     "segment"          +16   native unsigned short
%     "user"             +18   [8] native unsigned short
%  } 34 bytes
%  "user_int"         +296  [8] native int
%  "user_float"       +328  [8] native float
%} 360 bytes
%"traj"             +360  variable length of native float
%"data"             +376  variable length of native float
%} 392 bytes
