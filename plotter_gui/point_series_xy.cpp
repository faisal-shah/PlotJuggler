#include "point_series_xy.h"


PointSeriesXY::PointSeriesXY(const PlotData *y_axis, const PlotData *x_axis,
                             double time_offset):
    DataSeriesBase(time_offset),
    _x_axis(x_axis),
    _y_axis(y_axis)
{
    if( !updateCache() )
    {
        throw std::runtime_error("X and Y axis don't share the same time");
    }
}

nonstd::optional<QPointF> PointSeriesXY::sampleFromTime(double t)
{
    if( _cached_curve.size() == 0 )
    {
        return nonstd::optional<QPointF>();
    }

    int index = _y_axis->getIndexFromX(t);
    if(index <0)
    {
        return nonstd::optional<QPointF>();
    }
    const auto& p = _cached_curve.at( size_t(index) );
    return QPointF(p.x, p.y);
}

PlotData::RangeValueOpt PointSeriesXY::getVisualizationRangeY(PlotData::RangeTime range_X)
{
    //TODO: improve?
    return PlotData::RangeValueOpt( { _bounding_box.bottom(),
                                      _bounding_box.top() } );
}

bool PointSeriesXY::updateCache()
{
    const size_t data_size =  _x_axis->size();

    if( data_size != _x_axis->size() )
    {
        _bounding_box = QRectF();
        _cached_curve.clear();
        return false;
    }

    if(data_size == 0)
    {
        _bounding_box = QRectF();
        _cached_curve.clear();
        return true;
    }

    double min_y =( std::numeric_limits<double>::max() );
    double max_y =(-std::numeric_limits<double>::max() );
    double min_x =( std::numeric_limits<double>::max() );
    double max_x =(-std::numeric_limits<double>::max() );

    _cached_curve.resize( data_size );

    const double EPS = std::numeric_limits<double>::epsilon();

    for (size_t i=0; i<data_size; i++ )
    {
        if( std::abs( _x_axis->at(i).x  - _y_axis->at(i).x ) >  EPS)
        {
            _bounding_box = QRectF();
            _cached_curve.clear();
            return false;
        }

        const QPointF p(_x_axis->at(i).y,
                        _y_axis->at(i).y );

        _cached_curve.at(i) = { p.x(), p.y() };

        min_x = std::min( min_x, p.x() );
        max_x = std::max( max_x, p.x() );
        min_y = std::min( min_y, p.y() );
        max_y = std::max( max_y, p.y() );
    }

    _bounding_box.setLeft(  min_x );
    _bounding_box.setRight( max_x );
    _bounding_box.setBottom( min_y );
    _bounding_box.setTop( max_y );

    return true;
}
