classdef IsmrmrdDataset

    properties
        filename = '';
        dsetname = '';
        file = 0;

        memtype_acqhdr = 0;
        filetype_acqhdr = 0;
        space_acqhdr = 0;

        uint16_t = H5T.copy('H5T_NATIVE_UINT16');
        uint32_t = H5T.copy('H5T_NATIVE_UINT32');
        int32_t  = H5T.copy('H5T_NATIVE_INT32');
        uint64_t = H5T.copy('H5T_NATIVE_UINT64');
        float_t  = H5T.copy('H5T_NATIVE_FLOAT');

    end

methods

function obj = IsmrmrdDataset(filename, dsetname)

    if (nargin == 2)
        obj.filename = filename;
        obj.dsetname = dsetname;

        try
            file = H5F.open(filename);
            %dset = H5D.open(file, dsetname);
        catch
            file = H5F.create(filename, 'H5F_ACC_TRUNC', 'H5P_DEFAULT', 'H5P_DEFAULT');
            %dset = H5D.create(file, dsetname, );
        end

        obj.file = file;

        % Acquisition Header
        size_acqhdr(1) = H5T.get_size(obj.uint16_t); % version
        size_acqhdr(2) = H5T.get_size(obj.uint64_t); % flags
        size_acqhdr(3) = H5T.get_size(obj.uint32_t); % measurement_uid
        size_acqhdr(4) = H5T.get_size(obj.uint32_t); % scan_counter
        sizeof_acqhdr = sum(size_acqhdr);
        offsets_acqhdr(1) = 0;
        offsets_acqhdr(2:4) = cumsum(size_acqhdr(1:3));

        memtype_acqhdr = H5T.create('H5T_COMPOUND', sizeof_acqhdr);
        H5T.insert(memtype_acqhdr, 'version', offsets_acqhdr(1), obj.uint16_t);
        H5T.insert(memtype_acqhdr, 'flags', offsets_acqhdr(2), obj.uint64_t);
        H5T.insert(memtype_acqhdr, 'measurement_uid', offsets_acqhdr(3), obj.uint32_t);
        H5T.insert(memtype_acqhdr, 'scan_counter', offsets_acqhdr(4), obj.uint32_t);

        filetype_acqhdr = H5T.create('H5T_COMPOUND', sizeof_acqhdr);
        H5T.insert(filetype_acqhdr, 'version', offsets_acqhdr(1), obj.uint16_t);
        H5T.insert(filetype_acqhdr, 'flags', offsets_acqhdr(2), obj.uint64_t);
        H5T.insert(filetype_acqhdr, 'measurement_uid', offsets_acqhdr(3), obj.uint32_t);
        H5T.insert(filetype_acqhdr, 'scan_counter', offsets_acqhdr(4), obj.uint32_t);

        obj.memtype_acqhdr = memtype_acqhdr;
        obj.filetype_acqhdr = filetype_acqhdr;
        obj.space_acqhdr = H5S.create_simple(1, 1, []); % 1-D

    else
        error('IsmrmrdDataset takes 2 parameters (filename, dataset)');
    end
end

function obj = appendAcquisition(obj, hdr, data)
    dset = H5D.create(obj.file, obj.dsetname, obj.filetype_acqhdr, obj.space_acqhdr, 'H5P_DEFAULT');
    H5D.write(dset, obj.memtype_acqhdr, 'H5S_ALL', 'H5S_ALL', 'H5P_DEFAULT', hdr);
    H5D.close(dset)
end

function getAcquisition

end

function delete(obj)
    H5S.close(obj.space)
    H5T.close(obj.memtype_acqhdr)
    H5T.close(obj.datatype_acqhdr)
    H5F.close(obj.file)
end

end

end
