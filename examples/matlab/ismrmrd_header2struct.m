function s = ismrmrd_header2struct(header)
%
%
%  s = ismrmrd_header2struct(header)
%
%  Converts and ISMRMRD xml header to a struct.
%

    if (iscell(header)),
        header_string = header{1};
    else
        header_string = header;
    end

    if (ischar(header_string) < 1),
        error('Malformed input header. Is not a character string.');
    end

    tmp_nam = tempname;
    f=fopen(tmp_nam,'w');
    fwrite(f,header_string);
    fclose(f);

    s = xml2struct(tmp_nam);
return