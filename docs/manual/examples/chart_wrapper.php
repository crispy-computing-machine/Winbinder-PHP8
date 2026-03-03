<?php

/**
 * Lightweight helper for WinBinder ChartControl.
 *
 * Usage:
 * $chart = WBChart::create($window, ['x'=>10,'y'=>10,'w'=>400,'h'=>220]);
 * $chart->setSeries(0, [1,2,3])->setLabels(['A','B','C'])->refresh();
 */
class WBChart
{
    /** @var int WinBinder control handle */
    private $handle;

    /** @var int */
    private $style;

    /** @var int */
    private $events;

    private function __construct($handle, $style, $events)
    {
        $this->handle = $handle;
        $this->style = $style;
        $this->events = $events;
    }

    public static function create($parent, array $opts = [])
    {
        $id = isset($opts['id']) ? (int)$opts['id'] : 7001;
        $x = isset($opts['x']) ? (int)$opts['x'] : 10;
        $y = isset($opts['y']) ? (int)$opts['y'] : 10;
        $w = isset($opts['w']) ? (int)$opts['w'] : 600;
        $h = isset($opts['h']) ? (int)$opts['h'] : 300;

        $style = isset($opts['style']) ? (int)$opts['style'] : WBC_NOTIFY;
        $events = isset($opts['events'])
            ? (int)$opts['events']
            : (WBC_CHART_POINT_CLICK | WBC_CHART_POINT_HOVER);

        $handle = wb_create_control($parent, ChartControl, '', $x, $y, $w, $h, $id, $style, $events);
        if (!$handle) {
            throw new RuntimeException('Unable to create ChartControl.');
        }

        return new self($handle, $style, $events);
    }

    public function handle()
    {
        return $this->handle;
    }

    public function setSeries($seriesIndex, array $values)
    {
        wb_chart_set_series($this->handle, (int)$seriesIndex, self::floatArray($values));
        return $this;
    }

    public function setLabels(array $labels)
    {
        wb_chart_set_labels($this->handle, array_map('strval', $labels));
        return $this;
    }

    public function setYLabels(array $labels)
    {
        wb_chart_set_y_labels($this->handle, array_map('strval', $labels));
        return $this;
    }

    public function setColors(array $colors)
    {
        wb_chart_set_colors($this->handle, array_map('intval', $colors));
        return $this;
    }

    public function setAxis($showAxis = true, $showGrid = true, $min = 0.0, $max = 100.0, $autoRange = true)
    {
        wb_chart_set_axis($this->handle, (bool)$showAxis, (bool)$showGrid, (float)$min, (float)$max, (bool)$autoRange);
        return $this;
    }

    public function setPopup($enabled = true, $backgroundColor = 0xE1FFFF, $textColor = 0x000000, $borderColor = 0x5A5A5A)
    {
        wb_chart_set_popup($this->handle, (bool)$enabled, (int)$backgroundColor, (int)$textColor, (int)$borderColor);
        return $this;
    }

    public function refresh()
    {
        wb_chart_refresh($this->handle);
        return $this;
    }

    public static function pointFromEvent($param2)
    {
        return ((int)$param2) & 0xFFFF;
    }

    public static function seriesFromEvent($param2)
    {
        return (((int)$param2) >> 16) & 0xFFFF;
    }

    private static function floatArray(array $values)
    {
        return array_map('floatval', $values);
    }
}
