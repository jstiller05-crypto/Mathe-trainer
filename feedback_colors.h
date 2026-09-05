#ifndef FEEDBACK_COLORS_H
#define FEEDBACK_COLORS_H

// Gemeinsame Rueckmeldefarben fuer richtige/falsche Antworten - vorher als zwei
// hartkodierte Kopien in written_grid_widget.cpp UND task_view.cpp, jetzt an einer
// Stelle definiert. Beide Werte sind bewusst kraeftig genug gewaehlt, um sowohl auf
// dem dunklen (#1E1E1E) als auch dem hellen (#FAFAFA) App-Hintergrund gut lesbar zu
// sein (siehe MainWindow-Stylesheet fuer die beiden Hintergrundfarben).
inline constexpr const char *kCorrectAnswerColor = "#2ecc71";
inline constexpr const char *kWrongAnswerColor = "#e74c3c";

#endif
