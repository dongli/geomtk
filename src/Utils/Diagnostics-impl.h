namespace geomtk {

template <class MeshType, template <typename, int> class FieldType, class IOManagerType>
MeshType* Diagnostics<MeshType, FieldType, IOManagerType>::_mesh;
template <class MeshType, template <typename, int> class FieldType, class IOManagerType>
IOManagerType* Diagnostics<MeshType, FieldType, IOManagerType>::_io;
template <class MeshType, template <typename, int> class FieldType, class IOManagerType>
map<string, any> Diagnostics<MeshType, FieldType, IOManagerType>::_metrics;
template <class MeshType, template <typename, int> class FieldType, class IOManagerType>
int Diagnostics<MeshType, FieldType, IOManagerType>::outputIdx;

template <class MeshType, template <typename, int> class FieldType, class IOManagerType>
void Diagnostics<MeshType, FieldType, IOManagerType>::
init(MeshType &mesh, IOManagerType &io) {
    _mesh = &mesh;
    _io = &io;
    outputIdx = _io->addOutputFile(*_mesh, "diag.%5s.nc", TimeStepUnit::STEP, 1);
} // init

template <class MeshType, template <typename, int> class FieldType, class IOManagerType>
template <typename MetricDataType>
void Diagnostics<MeshType, FieldType, IOManagerType>::
addMetric(const string &name, const string &units, const string &longName) {
    MetricDataType *data = new MetricDataType;
    if (is_same<MetricDataType, CartesianField<int> >::value ||
        is_same<MetricDataType, RLLField<int> >::value) {
        data->create(name, units, longName, *_mesh, MeshType::Location::CENTER, _mesh->domain().numDim());
        _io->file(outputIdx).addField("int", IOManagerType::DataFileType::SpaceDimensions::FULL_DIMENSION, {data});
    }
    _metrics[name] = data;
} // addMetric

template <class MeshType, template <typename, int> class FieldType, class IOManagerType>
template <typename MetricDataType>
void Diagnostics<MeshType, FieldType, IOManagerType>::
resetMetric(const string &name) {
#ifndef NDEBUG
    if (_metrics.find(name) == _metrics.end()) {
        REPORT_ERROR("Invalid metric name \"" << name << "\"!");
    }
#endif
    MetricDataType& data = *any_cast<MetricDataType*>(_metrics[name]);
    if (is_same<MetricDataType, CartesianField<int> >::value ||
        is_same<MetricDataType, RLLField<int> >::value) {
        for (int i = 0; i < _mesh->totalNumGrid(data.staggerLocation()); ++i) {
            data(i) = 0;
        }
    }
} // resetMetric

template <class MeshType, template <typename, int> class FieldType, class IOManagerType>
template <typename MetricDataType>
MetricDataType& Diagnostics<MeshType, FieldType, IOManagerType>::
metric(const string &name) {
#ifndef NDEBUG
    if (_metrics.find(name) == _metrics.end()) {
        REPORT_ERROR("Invalid metric name \"" << name  << "\"!");
    }
#endif
    return *any_cast<MetricDataType*>(_metrics[name]);
} // metric

template <class MeshType, template <typename, int> class FieldType, class IOManagerType>
void Diagnostics<MeshType, FieldType, IOManagerType>::
output() {
    _io->create(outputIdx);
    for (auto metric : _metrics) {
        if (metric.second.type() == typeid(CartesianField<int>*) ||
            metric.second.type() == typeid(RLLField<int>*)) {
            _io->template output<int>(outputIdx, {any_cast<FieldType<int, 1>*>(metric.second)});
        }
    }
    _io->close(outputIdx);
} // output

} // geomtk
