function [xml_doc] = serialize( header)
%SERIALIZE Summary of this function goes here
%   Detailed explanation goes here
docNode = com.mathworks.xml.XMLUtils.createDocument('ismrmrdHeader');
docRootNode = docNode.getDocumentElement;
docRootNode.setAttribute('xmlns','http://www.ismrm.org/ISMRMRD');
docRootNode.setAttribute('xmlns:xsi','http://www.w3.org/2001/XMLSchema-instance');
docRootNode.setAttribute('xmlns:xs','http://www.w3.org/2001/XMLSchema');

docRootNode.setAttribute('xsi:schemaLocation','http://www.ismrm.org/ISMRMRD ismrmrd.xsd');

append_optional(docNode,docRootNode,header,'version',@int2str)

if isfield(header,'subjectInformation')
    subjectInformation = header.subjectInformation;
    subjectInformationNode = docNode.createElement('subjectInformation');
    append_optional(docNode,subjectInformationNode,subjectInformation,'patientName');
    append_optional(docNode,subjectInformationNode,subjectInformation,'patientWeight_kg',@num2str);
    append_optional(docNode,subjectInformationNode,subjectInformation,'patientHeight_m',@num2str);
    append_optional(docNode,subjectInformationNode,subjectInformation,'patientID');
    append_optional(docNode,subjectInformationNode,subjectInformation,'patientBirthdate');
    append_optional(docNode,subjectInformationNode,subjectInformation,'patientGender');
    docRootNode.appendChild(subjectInformationNode);
end

if isfield(header,'studyInformation')
    studyInformation = header.studyInformation;
    studyInformationNode = docNode.createElement('studyInformation');
    append_optional(docNode,studyInformationNode,studyInformation,'studyDate');
    append_optional(docNode,studyInformationNode,studyInformation,'studyTime');
    append_optional(docNode,studyInformationNode,studyInformation,'studyID');
    append_optional(docNode,studyInformationNode,studyInformation,'accessionNumber',@int2str);
    append_optional(docNode,studyInformationNode,studyInformation,'referringPhysicianName');
    append_optional(docNode,studyInformationNode,studyInformation,'studyDescription');
    append_optional(docNode,studyInformationNode,studyInformation,'studyInstanceUID');
    docRootNode.appendChild(studyInformationNode);
end

if isfield(header,'measurementInformation')
    measurementInformation = header.measurementInformation;
    measurementInformationNode = docNode.createElement('measurementInformation');
    append_optional(docNode,measurementInformationNode,measurementInformation,'measurementID');
    append_optional(docNode,measurementInformationNode,measurementInformation,'seriesDate');
    append_optional(docNode,measurementInformationNode,measurementInformation,'seriesTime');

    append_node(docNode,measurementInformationNode,measurementInformation,'patientPosition');
    append_optional_three_dimensional_float(docNode,measurementInformationNode,measurementInformation,'relativeTablePosition');

    append_optional(docNode,measurementInformationNode,measurementInformation,'initialSeriesNumber',@int2str);
    append_optional(docNode,measurementInformationNode,measurementInformation,'protocolName');
    append_optional(docNode,measurementInformationNode,measurementInformation,'seriesDescription');

    if isfield(measurementInformation, 'measurementDependency')
        measurementDependency = measurementInformation.measurementDependency;
        for dep = measurementDependency(:)'
            node = docNode.createElement('measurementDependency');
            append_node(docNode,node,dep,'dependencyType');
            append_node(docNode,node,dep,'measurementID');
            measurementInformationNode.appendChild(node)
        end
    end

    append_optional(docNode,measurementInformationNode,measurementInformation,'seriesInstanceUIDRoot');
    append_optional(docNode,measurementInformationNode,measurementInformation,'frameOfReferenceUID');

    if isfield(measurementInformation, 'referencedImageSequence')
        referencedImageSequence = measurementInformation.referencedImageSequence;
        referencedImageSequenceNode = docNode.createElement('referencedImageSequence');
        for ref = referencedImageSequence(:)
            append_node(docNode,referencedImageSequenceNode,ref,'referencedSOPInstanceUID');
        end
        measurementInformationNode.appendChild(referencedImageSequenceNode)
    end

    docRootNode.appendChild(measurementInformationNode);
end

if isfield(header,'acquisitionSystemInformation')
    acquisitionSystemInformation = header.acquisitionSystemInformation;
    acquisitionSystemInformationNode = docNode.createElement('acquisitionSystemInformation');
    append_optional(docNode,acquisitionSystemInformationNode,acquisitionSystemInformation,'systemVendor');
    append_optional(docNode,acquisitionSystemInformationNode,acquisitionSystemInformation,'systemModel');
    append_optional(docNode,acquisitionSystemInformationNode,acquisitionSystemInformation,'systemFieldStrength_T',@num2str);
    append_optional(docNode,acquisitionSystemInformationNode,acquisitionSystemInformation,'relativeReceiverNoiseBandwidth',@num2str);
    append_optional(docNode,acquisitionSystemInformationNode,acquisitionSystemInformation,'receiverChannels',@int2str);

    if isfield(acquisitionSystemInformation, 'coilLabel')
        coilLabel = acquisitionSystemInformation.coilLabel;
        for coil = 1:length(coilLabel)
            coilLabelNode = docNode.createElement('coilLabel');
            append_node(docNode,coilLabelNode,coilLabel(coil),'coilNumber',@num2str);
            append_node(docNode,coilLabelNode,coilLabel(coil),'coilName');
            acquisitionSystemInformationNode.appendChild(coilLabelNode);
        end
    end

    append_optional(docNode,acquisitionSystemInformationNode,acquisitionSystemInformation,'institutionName');
    append_optional(docNode,acquisitionSystemInformationNode,acquisitionSystemInformation,'stationName',@num2str);
    append_optional(docNode,acquisitionSystemInformationNode,acquisitionSystemInformation,'deviceID',@num2str);
    docRootNode.appendChild(acquisitionSystemInformationNode);
end

experimentalConditions = header.experimentalConditions;
experimentalConditionsNode = docNode.createElement('experimentalConditions');
append_node(docNode,experimentalConditionsNode,experimentalConditions,'H1resonanceFrequency_Hz',@int2str);
docRootNode.appendChild(experimentalConditionsNode);

if ~isfield(header,'encoding')
    error('Illegal header: missing encoding section');
end

for enc = header.encoding(:)'
    node = docNode.createElement('encoding');

    append_encoding_space(docNode,node,'encodedSpace',enc.encodedSpace);
    append_encoding_space(docNode,node,'reconSpace',enc.reconSpace);

    n2 = docNode.createElement('encodingLimits');

    append_encoding_limits(docNode,n2,'kspace_encoding_step_0',enc.encodingLimits);
    append_encoding_limits(docNode,n2,'kspace_encoding_step_1',enc.encodingLimits);
    append_encoding_limits(docNode,n2,'kspace_encoding_step_2',enc.encodingLimits);
    append_encoding_limits(docNode,n2,'average',enc.encodingLimits);
    append_encoding_limits(docNode,n2,'slice',enc.encodingLimits);
    append_encoding_limits(docNode,n2,'contrast',enc.encodingLimits);
    append_encoding_limits(docNode,n2,'phase',enc.encodingLimits);
    append_encoding_limits(docNode,n2,'repetition',enc.encodingLimits);
    append_encoding_limits(docNode,n2,'set',enc.encodingLimits);
    append_encoding_limits(docNode,n2,'segment',enc.encodingLimits);
    append_encoding_limits(docNode,n2,'user_0',enc.encodingLimits);
    append_encoding_limits(docNode,n2,'user_1',enc.encodingLimits);
    append_encoding_limits(docNode,n2,'user_2',enc.encodingLimits);
    append_encoding_limits(docNode,n2,'user_3',enc.encodingLimits);
    append_encoding_limits(docNode,n2,'user_4',enc.encodingLimits);
    append_encoding_limits(docNode,n2,'user_5',enc.encodingLimits);
    append_encoding_limits(docNode,n2,'user_6',enc.encodingLimits);
    append_encoding_limits(docNode,n2,'user_7',enc.encodingLimits);

    node.appendChild(n2);

    append_node(docNode,node,enc,'trajectory');

    % sometimes the encoding has the fields, but they are empty
    if isfield(enc,'trajectoryDescription')
        if ~isempty(fieldnames(enc.trajectoryDescription))
            n2 = docNode.createElement('trajectoryDescription');
            append_node(docNode,n2,enc.trajectoryDescription,'identifier');
            append_user_parameter(docNode,n2,enc.trajectoryDescription,'userParameterLong',@int2str);
            append_user_parameter(docNode,n2,enc.trajectoryDescription,'userParameterDouble',@num2str);
            append_optional(docNode,n2,enc.trajectoryDescription,'comment');
            node.appendChild(n2);
        end
    end

    if isfield(enc,'parallelImaging')
        if ~isempty(fieldnames(enc.parallelImaging))
            n2 = docNode.createElement('parallelImaging');

            n3 = docNode.createElement('accelerationFactor');
            parallelImaging = enc.parallelImaging;
            append_node(docNode,n3,parallelImaging.accelerationFactor,'kspace_encoding_step_1',@int2str);
            append_node(docNode,n3,parallelImaging.accelerationFactor,'kspace_encoding_step_2',@int2str);
            n2.appendChild(n3);

            append_optional(docNode,n2,parallelImaging,'calibrationMode');
            append_optional(docNode,n2,parallelImaging,'interleavingDimension',@int2str);

            node.appendChild(n2);
        end
    end

    if isfield(enc,'echoTrainLength')
        if ~isempty(enc.echoTrainLength)
            append_optional(docNode,node,enc,'echoTrainLength',@int2str);
        end
    end

    docRootNode.appendChild(node);

end

if isfield(header,'sequenceParameters')
    n1 = docNode.createElement('sequenceParameters');
    sequenceParameters = header.sequenceParameters;

    append_optional(docNode,n1,sequenceParameters,'TR',@num2str);
    append_optional(docNode,n1,sequenceParameters,'TE',@num2str);
    append_optional(docNode,n1,sequenceParameters,'TI',@num2str);
    append_optional(docNode,n1,sequenceParameters,'flipAngle_deg',@num2str);
    append_optional(docNode,n1,sequenceParameters,'sequence_type');
    append_optional(docNode,n1,sequenceParameters,'echo_spacing',@num2str);
    docRootNode.appendChild(n1);
end

if isfield(header,'userParameters')
    append_user_parameters(docNode, docRootNode, header.userParameters);
end

if isfield(header,'waveformInformation')

    for waveformInformation = header.waveformInformation
        
        waveformInfoNode = docNode.createElement('waveformInformation');
        append_node(docNode,waveformInfoNode,waveformInformation,'waveformName');
        append_node(docNode,waveformInfoNode,waveformInformation,'waveformType');

        if isfield(waveformInformation,'userParameters')
            append_user_parameters(docNode,waveformInfoNode,waveformInformation.userParameters)
        end 
        
        docRootNode.appendChild(waveformInfoNode);
    end            
end



xml_doc = xmlwrite(docNode);




end

function append_user_parameters(docNode, destination, userParams)

    userParametersNode = docNode.createElement('userParameters');

    for userParameters = userParams

        if isfield(userParameters,'userParameterLong')
            append_user_parameter(docNode,userParametersNode,userParameters,'userParameterLong',@int2str);
        end
        if isfield(userParameters,'userParameterDouble')
            append_user_parameter(docNode,userParametersNode,userParameters,'userParameterDouble',@num2str);
        end
        if isfield(userParameters,'userParameterString')
            append_user_parameter(docNode,userParametersNode,userParameters,'userParameterString');
        end
        if isfield(userParameters,'userParameterBase64')
            append_user_parameter(docNode,userParametersNode,userParameters,'userParameterBase64');
        end
        
    end

    destination.appendChild(userParametersNode);
end

function append_user_parameter(docNode,subNode,values,name,tostr)

for v = 1:length(values.(name))
    n2 = docNode.createElement(name);

    append_node(docNode,n2,values.(name)(v),'name');

    if nargin > 4
        append_node(docNode,n2,values.(name)(v),'value',tostr);
    else
        append_node(docNode,n2,values.(name)(v),'value');
    end

    subNode.appendChild(n2);
end
end


function append_encoding_limits(docNode,subNode,name,limit)
    if isfield(limit,name)
        n2 = docNode.createElement(name);
        append_node(docNode,n2,limit.(name),'minimum',@int2str);
        append_node(docNode,n2,limit.(name),'maximum',@int2str);
        append_node(docNode,n2,limit.(name),'center',@int2str);
        subNode.appendChild(n2);
    end
end

function append_encoding_space(docNode,subnode,name,encodedSpace)
    n2 = docNode.createElement(name);

    n3 = docNode.createElement('matrixSize');
    append_node(docNode,n3,encodedSpace.matrixSize,'x',@int2str);
    append_node(docNode,n3,encodedSpace.matrixSize,'y',@int2str);
    append_node(docNode,n3,encodedSpace.matrixSize,'z',@int2str);
    n2.appendChild(n3);

    n3 = docNode.createElement('fieldOfView_mm');
    append_node(docNode,n3,encodedSpace.fieldOfView_mm,'x',@num2str);
    append_node(docNode,n3,encodedSpace.fieldOfView_mm,'y',@num2str);
    append_node(docNode,n3,encodedSpace.fieldOfView_mm,'z',@num2str);
    n2.appendChild(n3);

    subnode.appendChild(n2);
end

function append_optional_three_dimensional_float(docNode, subnode, subheader, name)
    if isfield(subheader,name)
        n2 = docNode.createElement(name);
        threeDimensionalFloat = subheader.(matlab.lang.makeValidName(name));
        append_optional(docNode,n2,threeDimensionalFloat,'x',@num2str);
        append_optional(docNode,n2,threeDimensionalFloat,'y',@num2str);
        append_optional(docNode,n2,threeDimensionalFloat,'z',@num2str);
        subnode.appendChild(n2);
    end
end

function append_optional(docNode,subnode,subheader,name,tostr)
    if isfield(subheader,name)
        if nargin > 4
            append_node(docNode,subnode,subheader,name,tostr);
        else
            append_node(docNode,subnode,subheader,name);
        end
    end
end

function append_node(docNode,subnode,subheader,name,tostr)
    if ~exist('tostr', 'var')
        tostr = @char;
    end

    if ischar(subheader.(name))
        n1 = docNode.createElement(name);
        n1.appendChild...
        (docNode.createTextNode(subheader.(name)));
        subnode.appendChild(n1);
    else

        val = subheader.(name)(:);
        for thisval = 1:length(val)
            n1 = docNode.createElement(name);
            if iscell(val)
	            n1.appendChild(docNode.createTextNode(tostr(val{thisval})));
            else
	            n1.appendChild(docNode.createTextNode(tostr(val(thisval))));
            end
            subnode.appendChild(n1);
        end
    end
end
