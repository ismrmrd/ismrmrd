function tests = xmlTest
    tests = functiontests(localfunctions);
end

function setupOnce(test)

    test.TestData.headers = containers.Map('KeyType', 'char', 'ValueType', 'char');

    info = dir("xml/*.xml");        
    for index = 1:numel(info)
        
        finfo = info(index);
        
        disp("Reading xml header: " + finfo.name);

        fID = fopen(fullfile(finfo.folder, finfo.name), 'r');
        test.TestData.headers(finfo.name) = fread(fID, '*char');
        fclose(fID);       
    end
end

function setup(~)
    addpath('..');
end

function teardown(~)
    rmpath('..');
end

function testCanDeserializeMinimalHeader(test)
    header = test.TestData.headers('minimal.xml');    
    ismrmrd.xml.deserialize(header);
end

function testCanAddWaveformInformation(test)
    header = ismrmrd.xml.deserialize(test.TestData.headers('minimal.xml'));
    header.waveformInformation.waveformName = 'wisdoms';
    header.waveformInformation.waveformType = 'other';
    
    updated = ismrmrd.xml.serialize(header);
    updated = ismrmrd.xml.deserialize(updated);
       
    test.assertEqual(updated, header);
end

function testCanAddMultipleWaveformInformation(test)
    header = ismrmrd.xml.deserialize(test.TestData.headers('minimal.xml'));
    header.waveformInformation = [ ... 
        struct("waveformName", 'wisdoms', "waveformType", 'other'), ...
        struct("waveformName", 'boring-normal-waveforms', "waveformType", 'ecg') ...
    ];

    updated = ismrmrd.xml.serialize(header);
    updated = ismrmrd.xml.deserialize(updated);
    
    test.assertEqual(updated, header);
end

function testCanAddWaveformInformationWithUserParams(test)
    header = ismrmrd.xml.deserialize(test.TestData.headers('minimal.xml'));
    header.waveformInformation =  ...
        struct("waveformName", 'wisdoms', ...
               "waveformType", 'other', ...
               "userParameters", struct( ...
                   "userParameterString", [ ...
                        struct('name', 'line_1', 'value', 'The best wisdoms'), ...
                        struct('name', 'line_2', 'value', 'are often found'), ...
                        struct('name', 'line_3', 'value', 'in fortune cookies.') ...
                    ], ...
                    "userParameterLong", [ ...
                        struct('name', 'zero', 'value', 0), ...
                        struct('name', 'an_answer', 'value', 42) ...
                    ] ...
               ) ...
        );

    updated = ismrmrd.xml.serialize(header);       
    updated = ismrmrd.xml.deserialize(updated);
    
    test.assertEqual(updated, header);
end

function testCanAddMultipleWaveformInformationWithUserParams(test)
    header = ismrmrd.xml.deserialize(test.TestData.headers('ismrmrd_example.xml'));
    header.waveformInformation = [ ...
        struct("waveformName", 'wisdoms', "waveformType", 'other', ...
               "userParameters", struct( ...
                   "userParameterString", [ ...
                        struct('name', 'line_1', 'value', 'The best wisdoms'), ...
                        struct('name', 'line_2', 'value', 'are often found'), ...
                        struct('name', 'line_3', 'value', 'in fortune cookies.') ...
                    ] ...
               ) ...
        ), ...
        struct("waveformName", 'wisdoms', "waveformType", 'other', ...
               "userParameters", struct( ...
                    "userParameterLong", [ ...
                        struct('name', 'zero', 'value', 0), ...
                        struct('name', 'an_answer', 'value', 42) ...
                    ] ...
               ) ...
        ) ...
    ];

    updated = ismrmrd.xml.serialize(header);   
    updated = ismrmrd.xml.deserialize(updated);
    
    test.assertEqual(updated, header, 'RelTol', 1e-6);
end

function testCanWriteParallelImaging(test)
    header = ismrmrd.xml.deserialize(test.TestData.headers('minimal.xml'));

    header.encoding.parallelImaging.calibrationMode = 'other';
    header.encoding.parallelImaging.accelerationFactor.kspace_encoding_step_1 = 1;
    header.encoding.parallelImaging.accelerationFactor.kspace_encoding_step_2 = 1;
    
    updated = ismrmrd.xml.serialize(header);
    updated = ismrmrd.xml.deserialize(updated);

    test.assertEqual(updated, header);
end

function testCanReadMinimalWithUserParams(test)
    header = ismrmrd.xml.deserialize(test.TestData.headers('minimal_with_user_parameters.xml'));

    written = ismrmrd.xml.serialize(header);
    written = ismrmrd.xml.deserialize(written);
    
    test.assertEqual(written, header, 'RelTol', 1e-6);    
end

function testCanReadExample(test)
    header = ismrmrd.xml.deserialize(test.TestData.headers('ismrmrd_example.xml'));
    
    written = ismrmrd.xml.serialize(header);
    written = ismrmrd.xml.deserialize(written);
    
    test.assertEqual(written, header, 'RelTol', 1e-6);    
end

function testCanAddMixedUserParams(test)
    header = ismrmrd.xml.deserialize(test.TestData.headers('minimal.xml'));
    header.userParameters = struct( ...
        "userParameterString", [ ...
            struct('name', 'line_1', 'value', 'The best wisdoms'), ...
            struct('name', 'line_2', 'value', 'are often found'), ...
            struct('name', 'line_3', 'value', 'in fortune cookies.') ...
        ], ...
        "userParameterLong", [ ...
            struct('name', 'zero', 'value', 0), ...
            struct('name', 'an_answer', 'value', 42) ...
        ] ...
    );

    updated = ismrmrd.xml.serialize(header);
    updated = ismrmrd.xml.deserialize(updated);

    test.assertEqual(updated, header);
end
